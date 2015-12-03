#ifndef CURRENCY_STORAGE_HPP_INCLUDED
#define CURRENCY_STORAGE_HPP_INCLUDED

#include <map>
#include <mutex>
#include <string>

namespace storage {

    class CurrencyStorage
    {
        std::mutex staticStorageMutex;
        std::map<std::string, float> currentCurrency;
    public:
        CurrencyStorage() 
        {
            currentCurrency["BYR"] = 18200;
            currentCurrency["RUB"] = 67.2;
        }

        void updateCurrentCurrency(const std::map<std::string, float> &currency)
        {
            std::lock_guard<std::mutex> lock(staticStorageMutex);
            for (auto &kvp : currency) {
                currentCurrency[kvp.first] = kvp.second;
            }
        }

        std::map<std::string, float> getCurrentCurrency()
        {
            std::lock_guard<std::mutex> lock(staticStorageMutex);
            return currentCurrency;
        }
    };

}

static storage::CurrencyStorage staticStorage;

#endif
