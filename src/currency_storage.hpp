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

            std::ofstream fout(dumpFilename);
            fout << saverCurency.size() << "\n";
            for (auto &kvp : saverCurency) {
                fout << kvp.first << "\n" << kvp.second << "\n";
            }
            fout.close();

            notifySaver = false;
        }
    }

    void saveAsync(const Currency &currency)
    {
        std::unique_lock<std::mutex> locker(saverMutex);
        notifySaver = true;
        saverCurency = currency;
        saverCondition.notify_one();
    }

    Currency load()
    {
        std::unique_lock<std::mutex> locker(saverMutex);
        Currency currency;
        try {
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
        } catch (const std::exception& e) {
            std::cerr << "Read dump error: " << e.what() << "\n";
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
            std::lock_guard<std::mutex> lock(staticStorageMutex);
            for (auto &kvp : currency) {
                currentCurrency[kvp.first] = kvp.second;
            }
            updateTime = boost::posix_time::microsec_clock::universal_time();
            saveAsync(currentCurrency);
        }

        Currency getCurrency(boost::posix_time::ptime *time)
        {
            std::lock_guard<std::mutex> lock(staticStorageMutex);
            *time = updateTime;
            return currentCurrency;
        }
    };

}

static storage::CurrencyStorage staticStorage;

#endif
