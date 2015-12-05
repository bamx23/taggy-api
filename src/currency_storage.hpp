#ifndef CURRENCY_STORAGE_HPP_INCLUDED
#define CURRENCY_STORAGE_HPP_INCLUDED

#include <boost/date_time/posix_time/posix_time.hpp>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <map>
#include <string>
#include <fstream>

#include "error.hpp"

namespace storage {

    typedef std::map<std::string, float> Currency;

    static const char dumpFilename[] = "currency.dump";
    std::condition_variable saverCondition;
    std::mutex saverMutex;
    bool notifySaver, closeSaver;
    Currency saverCurency;

    void asyncSaver() 
    {
        while (!closeSaver) {
            std::unique_lock<std::mutex> locker(saverMutex);
            saverCondition.wait(locker, []{ return notifySaver; });

            debug_log("Saving currency");
            std::ofstream fout(dumpFilename);
            fout << saverCurency.size() << "\n";
            for (auto &kvp : saverCurency) {
                fout << kvp.first << "\n" << kvp.second << "\n";
            }
            fout.close();
            debug_log("Saved currency");

            notifySaver = false;
        }
    }

    void saveAsync(const Currency &currency)
    {
        std::unique_lock<std::mutex> locker(saverMutex);
        notifySaver = true;
        saverCurency = currency;
        debug_log("Notify saving currency");
        saverCondition.notify_one();
    }

    inline bool fileExists(const std::string &name) {
        struct stat buffer;   
        return (stat(name.c_str(), &buffer) == 0); 
    }

    Currency load()
    {
        std::unique_lock<std::mutex> locker(saverMutex);
        Currency currency;
        if (fileExists(dumpFilename)) {
            try {
                debug_log("Loading currency");
                std::ifstream fin(dumpFilename);
                size_t count;
                fin >> count;
                for (size_t i = 0; i < count; ++i) {
                    std::string name;
                    float value;
                    fin >> name >> value;
                    currency[name] = value;
                }
                fin.close();
                debug_log("Loaded currency");
            } catch (const std::exception& e) {
                std::cerr << "Read dump error: " << e.what() << "\n";
            }
        }
        return currency;
    }

    class CurrencyStorage
    {
        std::mutex staticStorageMutex;
        Currency currentCurrency;
        boost::posix_time::ptime updateTime;

        std::thread saverThread;
    public:
        CurrencyStorage() : saverThread(asyncSaver)
        {
            currentCurrency = load();
            // Save/load time
            updateTime = boost::posix_time::microsec_clock::universal_time();
        }

        void updateCurrency(const Currency &currency)
        {
            debug_log("Update before lock");
            std::lock_guard<std::mutex> lock(staticStorageMutex);
            debug_log("Updating currency");
            for (auto &kvp : currency) {
                currentCurrency[kvp.first] = kvp.second;
            }
            updateTime = boost::posix_time::microsec_clock::universal_time();
            saveAsync(currentCurrency);
            debug_log("Updated currency");
        }

        Currency getCurrency(boost::posix_time::ptime *time)
        {
            debug_log("Get before lock");
            std::lock_guard<std::mutex> lock(staticStorageMutex);
            debug_log("Getting currency");
            *time = updateTime;
            return currentCurrency;
        }
    };

}

static storage::CurrencyStorage staticStorage;

#endif
