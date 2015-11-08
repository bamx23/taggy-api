#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

#include "error.hpp"
#include "template.hpp"
#include "json.hpp"

static std::wstring const kTGTitle = L"Taggy: Currency Converter - Currency API";

class BootstrapHtml : public Template
{
    virtual void header(tstream &out) const
    { 
        out << 
            "<html lang='en'>"
                "<head>"
                    "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
                    "<meta charset='utf-8'>"
                    "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
                    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                    "<meta name='author' content='Nikolay Volosatov'>"
                    "<title>" << kTGTitle << "</title>"
                    "<link rel='stylesheet' href='/static/css/bootstrap.min.css' />"
                    "<link rel='stylesheet' href='/static/css/bootstrap.theme.css' />"
                    "<link rel='stylesheet' href='/static/css/style.css' />"
                "</head>"
                "<body>";
    }

    virtual void footer(tstream &out) const
    { 
        out << 
                "<script src='/static/js/bootstrap.min.js'></script>"
                "</body>"
            "</html>";
    }
};

class MainPageBody : public Template
{
    virtual void header(tstream &out) const
    {
        out << 
            "<div class='container'>"
                "<div class='header'>"
                    "<nav>"
                        "<ul class='nav nav-pills pull-right'>"
                            "<li role='presentation' class='active'><a href='/'>API</a></li>"
                            "<li role='presentation'><a href='http://taggy.by/'>Home</a></li>"
                        "</ul>"
                    "</nav>"
                    "<h3 class='text-muted'>" << kTGTitle << "</h3>"
                "</div>"
                "<div class='jumbotron'>";
    }

    virtual void footer(tstream &out) const
    { 
        out <<
                "</div>"
                "<footer class='footer'>"
                "<p>&copy; VASAG 2015</p>"
                "</footer>"
            "</div> <!-- /container -->"; 
    }
};

class MainRequest : public Fastcgipp::Request<wchar_t>
{
    void httpHeader() 
    {
        out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
    }

    bool response()
    {
        httpHeader();

        BootstrapHtml html;
        MainPageBody body;
        
        if (environment().requestUri == L"/") {
            body << 
                "<p>"
                    "<a href='/rates' class='btn btn-sm btn-warning'>Latest Currency(JSON)</a> "
                    "<a href='/history/0/24' class='btn btn-sm btn-warning'>Currency History(JSON)</a>"
                "</p>"
                "<h3>Actual currencies <i>(for 1 USD)</i></h3>";

            body << 
                "<table class='table' style='text-align: center'>"
                    "<tr><td>Currency</td><td>Rate</td></tr>";
            for (size_t i = 0; i < 20; ++i) {
                body <<
                    "<tr><td><strong>" << "BYR" << "</strong></td><td>" << 17500.02 << "</td></tr>";
            }
            body <<
                "</table>";
        } else if (environment().requestUri == L"/json") {
            out << (jobj() 
                    << jkvp(L"key", 42)
                    << jkvp(L"string", L"str")
                    << jkvp(L"string", L"ecr\\str")
                    << jkvp(L"arr", jarr() 
                        << L"hello" 
                        << 23 
                        << 77.01f 
                        << -1L
                    )
                    << jkvp(L"obj", jobj() 
                        << jkvp(L"inKey", 0)
                        << jkvp(L"str", L"World!")
                    )
                );
            return true;
        } else {
            body << "<h1>404 Not found</h1>";
        }

        html << body;
        out << html;
        return true;
    }
};

// The main function is easy to set up
int main()
{
    try
    {
        Fastcgipp::Manager<MainRequest> fcgi;
        fcgi.handler();
    }
    catch(std::exception& e)
    {
        error_log(e.what());
    }
}