#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

#include "error.hpp"
#include "template.hpp"

static char *const kTGTitle = "Taggy: Currency Converter - Currency API";

class MainHtmlTemplate : public Template
{
    virtual void header(std::stringstream &out) const
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
                    "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css' integrity='sha512-dTfge/zgoMYpP7QbHy4gWMEGsbsdZeCXz7irItjcC3sPUFtf0kuFbDz/ixG7ArTxmDjLXDmezHubeNikyKGVyQ==' crossorigin='anonymous' />"
                    // "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css' integrity='sha384-aUGj/X2zp5rLCbBxumKTCw2Z50WgIr1vs/PFN4praOTvYXWlVyh2UtNUU0KAUhAX' crossorigin='anonymous' />"
                    "<link rel='stylesheet' href='http://bootswatch.com/cosmo/bootstrap.min.css' />"
                    "<link rel='stylesheet' href='http://getbootstrap.com/examples/jumbotron-narrow/jumbotron-narrow.css' />"
                "</head>"
                "<body>";
    }

    virtual void footer(std::stringstream &out) const
    { 
        out << 
                "<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js' integrity='sha512-K1qjQ+NcF2TYO/eI3M6v8EiNYZfA95pQumfvcVrTHtwQVDG+aHRqLi/ETn2uB+1JqwYqVG3LIvdm9lj6imS/pQ==' crossorigin='anonymous'></script>"
                "</body>"
            "</html>";
    }
};

class ListCurrencyTemplate : public Template
{
    virtual void header(std::stringstream &out) const
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

    virtual void footer(std::stringstream &out) const
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

        MainHtmlTemplate body;
        ListCurrencyTemplate curDiv;

        curDiv << "Hello from Template! <br />";
        curDiv << "And it works! " << 42 << " == 42!";

        body << curDiv;
        dumpTo(out, body);

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