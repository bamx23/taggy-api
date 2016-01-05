#ifndef CURRENCY_STORAGE_HPP_INCLUDED
#define CURRENCY_STORAGE_HPP_INCLUDED

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/conversion.hpp>

#include <sys/stat.h>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <vector>
#include <string>
#include <fstream>

#include "../utility/error.hpp"
#include "../boost-fix/ptree-fix.hpp"
#include "../boost-fix/json_parser.hpp"

namespace storage
{
    static const char dumpFilename[] = "/data/currency.dump";

    struct Rate
    {
        std::string name;
        float value;
        boost::posix_time::ptime updateTime;

        Rate() : value(0) { };

        Rate(const Rate &other)
        {
            name = other.name;
            value = other.value;
            updateTime = other.updateTime;
        }

        void read(std::istream &stream)
        {
            std::time_t time;
            stream >> name >> value >> time;
            updateTime = boost::posix_time::from_time_t(time);
        }

        void write(std::ostream &stream) const
        {
            using namespace boost::posix_time;
            ptime start(boost::gregorian::date(1970, 1, 1));
            stream << name << "\n" << value << "\n" << (updateTime - start).total_seconds() << "\n";
        }
    };

    typedef std::vector<Rate> Currency;

    std::mutex saverMutex;

    void asyncSaver();
    void closeSaverAsync();
    void saveAsync(const Currency &);

    std::string utcDate(const boost::posix_time::ptime &time)
    {
        using namespace boost::posix_time;
        static std::locale loc(std::locale::classic(), new time_facet("%Y%m%dT%H%M%S"));

        std::ostringstream buffer;
        buffer.imbue(loc);
        buffer << time;
        return buffer.str();
    }

    class CurrencyStorage
    {
        std::mutex staticStorageMutex;
        Currency currentCurrency;
        std::string jsonString;

        std::thread saverThread;

        inline bool fileExists(const std::string &name) {
            struct stat buffer;
            return (stat(name.c_str(), &buffer) == 0);
        }

        void load()
        {
            std::unique_lock<std::mutex> locker(saverMutex);
            currentCurrency.clear();
            if (fileExists(dumpFilename)) {
                try {
                    debug_log("Loading currency");
                    std::ifstream fin(dumpFilename);
                    size_t count;
                    fin >> count;
                    for (size_t i = 0; i < count; ++i) {
                        Rate rate;
                        rate.read(fin);
                        currentCurrency.push_back(rate);
                    }
                    fin.close();
                    debug_log("Loaded currency");
                } catch (const std::exception& e) {
                    currentCurrency.clear();
                    std::cerr << "Read dump error: " << e.what() << "\n";
                }
            }
        }

        void updateJsonString()
        {
            using boost::property_tree::ptree;
            using boost::property_tree::write_json;

            ptree result;

            if (currentCurrency.size() > 0) {
                boost::posix_time::ptime updateTime = currentCurrency[0].updateTime;

                ptree currencyRates;
                for (auto &rate : currentCurrency) {
                    ptree element;
                    put_str(element, "name", rate.name);
                    element.put<float>("value", rate.value);
                    currencyRates.push_back(std::make_pair("", element));

                    if (rate.updateTime > updateTime) {
                        updateTime = rate.updateTime;
                    }
                }
                result.add_child("currency", currencyRates);

                put_str(result, "time", utcDate(updateTime));
            } else {
                // TODO: implement error json
            }

            std::stringstream buffer;
            write_json(buffer, result);
            jsonString = buffer.str();
        }

        boost::posix_time::ptime roundedTime(const boost::posix_time::ptime &time)
        {
            using namespace boost::posix_time;
            return ptime(time.date(), seconds(time.time_of_day().total_seconds()));
        }

    public:
        CurrencyStorage() : saverThread(asyncSaver)
        {
            load();
            updateJsonString();
        }

        ~CurrencyStorage()
        {
            closeSaverAsync();
            saverThread.join();
        }

        void updateCurrency(const std::map<std::string, float> &currency)
        {
            debug_log("Update before lock");
            std::lock_guard<std::mutex> lock(staticStorageMutex);
            debug_log("Updating currency");

            auto updateTime = roundedTime(boost::posix_time::microsec_clock::universal_time());
            for (auto &rate : currency) {
                int index = -1;
                for (size_t i = 0; i < currentCurrency.size(); ++i) {
                    if (rate.first == currentCurrency[i].name) {
                        index = (int)i;
                        break;
                    }
                }
                if (index == -1) {
                    Rate newRate;
                    newRate.name = rate.first;
                    currentCurrency.push_back(newRate);
                    index = (int)(currentCurrency.size() - 1);
                }

                auto &currentRate = currentCurrency[index];
                currentRate.value = rate.second;
                currentRate.updateTime = updateTime;
            }

            saveAsync(currentCurrency);
            updateJsonString();

            debug_log("Updated currency");
        }

        std::string getJson()
        {
            debug_log("Get before lock");
            //std::lock_guard<std::mutex> lock(staticStorageMutex);
            debug_log("Getting currency");
            return jsonString;
        }
    };

    std::condition_variable saverCondition;
    bool notifySaver, closeSaver;
    Currency saverCurency;

    void saveSync(const Currency &currency)
    {
        debug_log("Saving currency");

        std::ofstream fout(dumpFilename);
        fout << currency.size() << "\n";
        for (auto &rate : currency) {
            rate.write(fout);
        }
        fout.close();
        debug_log("Saved currency");
    }

    void asyncSaver()
    {
        while (!closeSaver) {
            std::unique_lock<std::mutex> locker(saverMutex);
            saverCondition.wait(locker, []{ return closeSaver || notifySaver; });

            if (closeSaver) return;

            saveSync(saverCurency);

            notifySaver = false;
        }
    }

    void closeSaverAsync()
    {
        std::unique_lock<std::mutex> locker(saverMutex);
        closeSaver = true;
        saverCondition.notify_one();
    }

    void saveAsync(const Currency &currency)
    {
        std::unique_lock<std::mutex> locker(saverMutex);
        notifySaver = true;
        saverCurency = currency;
        debug_log("Notify saving currency");
        saverCondition.notify_one();
    }

}

static storage::CurrencyStorage staticStorage;

#endif
