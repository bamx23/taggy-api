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

#endif
