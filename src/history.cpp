#include <fastcgi++/fcgistream.hpp>
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

#include "utility/error.hpp"
#include "utility/types.hpp"
#include "boost-fix/ptree-fix.hpp"
#include "boost-fix/json_parser.hpp"

#include "storage/history_storage.hpp"
#include "storage/history_updater.hpp"

using boost::property_tree::ptree;

static history_updater::Updater updater;

class MainRequest : public Fastcgipp::Request<tchar_t>
{
    std::string utcDate(const boost::posix_time::ptime &time)
    {
        using namespace boost::posix_time;
        static std::locale loc(std::locale::classic(), new time_facet("%Y%m%d-%H"));

        std::ostringstream buffer;
        buffer.imbue(loc);
        buffer << time;
        return buffer.str();
    }

    void httpHeader(bool isJson = true) 
    {
        if (isJson) {
            out << "Content-Type: application/json; charset=utf-8\r\n\r\n";
        } else {
            out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
        }
    }

    void http404()
    {
        debug_log("Called 404");
        out << "Status: 404 Not found\r\n";
        out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
    }

    void http500()
    {
        debug_log("Called 500");
        out << "Status: 500 Internal Server Error\r\n";
        out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
    }

    bool getHistory()
    {
        httpHeader(false);

        auto pathInfo = environment().pathInfo;
        boost::posix_time::ptime time = microsec_clock::universal_time();
        int count = 20;
        if (pathInfo.size() >= 1) {
            history_updater::dateparser dp("%Y%m%d-%H");
            dp(pathInfo[0].data());
            time = dp.pt;
            if (pathInfo.size() >= 2) {
                count = atoi(pathInfo[1].data());
                if (count > 100) count = 100;
                if (count < 1) count = 1;
            }
        }

        ptree root;
        ptree currencyRates;
        for (int i = 0; i < count; ++i) {
            auto curTime = time - boost::posix_time::hours(i);
            if (staticHistoryStorage.containsCurrency(curTime)) {
                auto cur = staticHistoryStorage.getCurrency(curTime);
                currencyRates.push_back(std::make_pair("", cur.getJson()));
            }
        }
        root.add_child("list", currencyRates);

        std::stringstream nextUrl;
        nextUrl << "\"" << environment().requestUri;
        nextUrl << "/" << utcDate(time - boost::posix_time::hours(count));
        nextUrl << "/" << count << "\"";
        root.put("next", nextUrl.str());

        std::stringstream selfUrl;
        selfUrl << "\"" << environment().requestUri;
        selfUrl << "/" << utcDate(time);
        selfUrl << "/" << count << "\"";
        root.put("self", selfUrl.str());

        std::stringstream idStr;
        idStr << utcDate(time) << "-" << count; 
        put_str(root, "id", idStr.str());
        
        std::stringstream buffer;
        write_json(buffer, root);
        out << buffer.str();
        return true;
    }

    bool response()
    {
        debug_log("===response start");
        switch (environment().requestMethod) {
            case Fastcgipp::Http::HTTP_METHOD_GET:
                if (environment().scriptName == "/api/v1/history") {
                    return getHistory();
                }
                break;

            default:
                break;
        }

        http404();
        out << "404 Not found";
        return true;
    }
};

int main()
{
    try {
        Fastcgipp::Manager<MainRequest> fcgi;
        fcgi.handler();
    } catch(std::exception& e) {
        error_log(e.what());
    }
}