#define XCODE

#include "currency_storage.hpp"

int entry_point()
{
    std::map<std::string, float> cur;
    cur["BYR"] = 18500;
    cur["RUB"] = 67;

    staticStorage.updateCurrency(cur);

    debug_log(staticStorage.getJson().data());
    return 0;
}
