#ifndef json_client_h
#define json_client_h

#include "../utility/error.hpp"
#include "../boost-fix/ptree-fix.hpp"
#include "../boost-fix/json_parser.hpp"
#include <boost/asio.hpp>

// Code from example:
// http://www.boost.org/doc/libs/1_38_0/doc/html/boost_asio/example/http/client/sync_client.cpp

using boost::asio::ip::tcp;

boost::property_tree::ptree http_get_json(const char *host, const char *path, int *response_code)
{
    boost::property_tree::ptree result;
    try
    {
        boost::asio::io_service io_service;

        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(host, "http");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        // Try each endpoint until we successfully establish a connection.
        tcp::socket socket(io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end) {
            socket.close();
            socket.connect(*endpoint_iterator++, error);
        }
        if (error) {
            throw boost::system::system_error(error);
        }

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << path << " HTTP/1.0\r\n";
        request_stream << "Host: " << host << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        boost::asio::write(socket, request);

        // Read the response status line.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            return result;
        }
        if (response_code != NULL) {
            *response_code = status_code;
        }
        if (status_code != 200) {
            return result;
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            //std::cout << header << "\n";
        }
        //std::cout << "\n";

        std::stringstream ss;

        // Write whatever content we already have to output.
        if (response.size() > 0) {
            ss << &response;
        }

        // Read until EOF, writing data to output as we go.
        while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
            ss << &response;
        }

        if (error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

        boost::property_tree::read_json(ss, result);
    } catch (std::exception& e) {
        error_log(e.what());
    }
    
    return result;
}

#endif
