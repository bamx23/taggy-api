#ifndef ERROR_HPP
#define ERROR_HPP

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>

void error_log(const char* msg)
{
    using namespace std;
    using namespace boost;
    static ofstream error;
    if (!error.is_open()) {
        error.open("/var/log/fastcgipp/hello.log", ios_base::out | ios_base::app);
        error.imbue(locale(error.getloc(), new posix_time::time_facet()));
    }

    error << '[' << posix_time::second_clock::local_time() << "] " << msg << endl;
}

void debug_log(const char* msg)
{
    using namespace std;
    using namespace boost;
    static ofstream debug;
    if (!debug.is_open()) {
        debug.open("/var/log/fastcgipp/debug.log", ios_base::out | ios_base::app);
        debug.imbue(locale(debug.getloc(), new posix_time::time_facet()));
    }

    debug << '[' << posix_time::second_clock::local_time() << "] " << msg << endl;
}

#endif
