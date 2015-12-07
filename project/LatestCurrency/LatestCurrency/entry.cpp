#define XCODE

#include "currency_storage.hpp"
#include "history_storage.hpp"

void test_currency_storage()
{
    std::map<std::string, float> cur;
    cur["BYR"] = 18700;
    cur["RUB"] = 67;

    staticStorage.updateCurrency(cur);

    debug_log(staticStorage.getJson().data());
}

void test_history_storage()
{
    std::map<std::string, float> cur1;
    cur1["BYR"] = 18200;
    cur1["RUB"] = 67;

    std::map<std::string, float> cur2;
    cur2["BYR"] = 18500;
    cur2["RUB"] = 72;

    boost::posix_time::ptime time1(from_iso_string("20150305T110710"));
    boost::posix_time::ptime time2(from_iso_string("20150305T122320"));

    staticHistoryStorage.addCurrency(time1, cur1);
    staticHistoryStorage.addCurrency(time2, cur2);

    auto ncur1 = staticHistoryStorage.getCurrency(time1);
    ncur1 = staticHistoryStorage.getCurrency(time1);
    ncur1 = staticHistoryStorage.getCurrency(time1);

    std::stringstream buffer;
    write_json(buffer, ncur1.getJson());

    debug_log(buffer.str().data());
}

int entry_point()
{
    test_currency_storage();
    test_history_storage();
    return 0;
}
