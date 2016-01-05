#ifndef history_updater_h
#define history_updater_h

#include <map>
#include <chrono>
#include <thread>

#include "../utility/json_client.hpp"
#include "../storage/history_storage.hpp"

namespace history_updater {

    using namespace boost::posix_time;
    using namespace boost::gregorian;

    struct dateparser
    {
        dateparser(std::string fmt) {
            // set format
            using namespace boost::posix_time;
            static std::locale loc(std::locale::classic(), new time_facet(fmt.data()));
            ss.imbue(loc);
        }

        void operator()(std::string const& text)
        {
            ss.clear();
            ss.str(text);

            ss >> pt;
        }
        
        ptime pt;
        
    private:
        std::stringstream ss;
    };

    static dateparser dp("%Y%m%dT%H%M%S");

    void update_sync() {
        int code = 500, repeats = 5;
        boost::property_tree::ptree root;

        while (code != 200 && repeats --> 0) {
            code = 500;
            root = http_get_json("latest", "/api/v1/latest/", &code);
            //root = http_get_json("taggy-api.bx23.net", "/api/v1/latest/", &code);
        }

        if (code != 200) {
            error_log("Fail to load currency");
            return;
        }

        std::map<std::string, float> rates;
        for (auto &kvp : root.get_child("currency")) {
            auto rate = kvp.second;
            auto name = rate.get<std::string>("name");
            auto value = rate.get<float>("value");
            rates[name] = value;
        }
        auto timeStr = root.get<std::string>("time");
        dp(timeStr);
        auto time = dp.pt;

        staticHistoryStorage.addCurrency(time, rates);
        debug_log("Currency history updated");
    }

    bool close_saver;
    ptime nextUpdateTime;

    ptime roundedTime(const ptime &time) {
        time_duration tod = seconds(time.time_of_day().total_seconds());
        time_duration roundedDownTod = hours(tod.hours());
        ptime result(time.date(), roundedDownTod);
        return result;
    }

    void update_time() {
        ptime now = second_clock::local_time();
        ptime rounded = roundedTime(now) + minutes(2);
        if (rounded <= now) {
            rounded += hours(1);
        }
        nextUpdateTime = rounded;
    }

    void update_loop() {
        close_saver = false;
        update_time();

        while (!close_saver) {
            ptime now = second_clock::local_time();
            if (now >= nextUpdateTime) {
                update_sync();
                update_time();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    class Updater
    {
        std::thread thread;

    public:
        Updater() : thread(update_loop) { }

        ~Updater()
        {
            close_saver = true;
            thread.join();
        }
    };

    static Updater updater;
}

#endif
