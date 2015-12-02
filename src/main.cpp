#include <fastcgi++/fcgistream.hpp>
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>
#include "ptree.hpp"
#include "json_parser.hpp"
#include <map>
#include <sstream>

#include "error.hpp"

typedef char tchar_t;
typedef std::basic_string<tchar_t> tstring;
typedef std::basic_stringstream<tchar_t, std::char_traits<tchar_t>, std::allocator<tchar_t> > tstream;
typedef Fastcgipp::Fcgistream<tchar_t> fcgistream;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

class CurrencyStorage
{
    std::map<std::string, float> currentCurrency;
public:
    CurrencyStorage() 
    {
        currentCurrency["BYR"] = 18200;
        currentCurrency["RUB"] = 67.2;
    }

    void updateCurrentCurrency(std::map<std::string, float> currency)
    {
        for (auto &kvp : currency) {
            currentCurrency[kvp.first] = kvp.second;
        }
    }

    std::map<std::string, float> &getCurrentCurrency()
    {
        return currentCurrency;
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

    void http500()
    {
        out << "Status: 500 Internal Server Error\r\n";
        out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
    }

    bool jsonGetCurrency()
    {
        auto currency = storage.getCurrentCurrency();
        if (currency.size() == 0) {
            http500();
            return true;
        }
        ptree result;

        ptree currencyRates;
        for (auto &kvp : currency) {
            ptree element;
            put_str(element, "name", kvp.first);
            element.put<float>("value", kvp.second);
            currencyRates.push_back(std::make_pair("", element));
        }
        result.add_child("currency", currencyRates);

        put_str(result, "id", "20151025T000300");
        put_str(result, "time", "20151025T000300");
        put_str(result, "updated", "20151025T000300");

        httpHeader();
        write_json(out, result);
        return true;
    }

    bool jsonUpdateCurrency()
    {
        httpHeader();

        for (auto &kvp : environment().posts) {
            out << kvp.first << " :<br/>";
            out << kvp.second.value;
        }

        

        return true;

        ptree root;
        std::istringstream dataStream(environment().findPost("data").value);
        read_json(dataStream, root);
        for (auto &rate : root.get_child("currency")) {
            out << rate.second.get_value("name");//<< " : " << rate.second.get_child("value").str();
        }
        return true;
    }

    bool response()
    {
        if (environment().requestMethod == Fastcgipp::Http::HTTP_METHOD_GET) {
            if (environment().requestUri == "/api/v1/currency/") {
                return jsonGetCurrency();
            } else {
                http404();
                out << "404 Not found";
            }
        } else if (environment().requestMethod == Fastcgipp::Http::HTTP_METHOD_POST) {
            return jsonUpdateCurrency();
            //if (environment().requestUri == "/api/v1/currency/") {
            //    
            //}
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