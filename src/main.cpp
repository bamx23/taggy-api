#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>
#include <map>

#include "error.hpp"
#include "template.hpp"
#include "json.hpp"

class CurrencyStorage
{
public:
    std::map<std::string, float> getCurrentCurrency()
    {
        std::map<std::string, float> result;
        result["BYR"] = 18000;
        result["RUB"] = 65;
        return result;
    }
};

class MainRequest : public Fastcgipp::Request<tchar_t>
{
    CurrencyStorage storage;

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
        out << "Status: 404 Not found\r\n";
        out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
    }

    bool response()
    {
        if (environment().requestUri == "/api/v1/currency/") {
            httpHeader();
            auto currency = storage.getCurrentCurrency();
            jarr currencyArr;

            for (auto& kvp : currency) {
                currencyArr << (jobj() 
                    << jkvp("name", kvp.first) 
                    << jkvp("value", kvp.second)
                );
            }

            out << (jobj() 
                    << jkvp("currency", currencyArr)
                    << jkvp("id", "20151025T000300")
                    << jkvp("time", "20151025T000300+03")
                    << jkvp("updated", "20151025T190300+03")
                );
        } else {
            http404();
            out << "404 Not found";
        }

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