#ifndef ERROR_HPP
#define ERROR_HPP

#include <fstream>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace logging 
{
    struct Message
    {
        std::string text;
        bool isError;

        Message(const char *msg, bool error) : text(msg), isError(error) { }
    };

    std::queue<Message> messages;
    std::condition_variable loggerCondition;
    std::mutex loggerMutex;

    void syncLog(const Message &msg)
    {
        using namespace std;
        using namespace boost;
        static ofstream debug;

        if (!debug.is_open()) {
            debug.open("/var/log/fastcgipp/debug.log", ios_base::out | ios_base::app);
            debug.imbue(locale(debug.getloc(), new posix_time::time_facet()));
        }

        debug << (msg.isError ? "E " : "D ") << '[' << posix_time::second_clock::local_time() << "] " << msg.text << endl;
    }

    void asyncLogger() 
    {
        while (true) {
            std::unique_lock<std::mutex> locker(loggerMutex);
            loggerCondition.wait(locker, []{ return !messages.empty(); });

            while (!messages.empty()) {
                syncLog(messages.front());
                messages.pop();
            }
        }
    }

    void logSync(const char *msg, bool isError)
    {
        std::unique_lock<std::mutex> locker(loggerMutex);
        syncLog(Message(msg, isError));
    }

    void logAsync(const char *msg, bool isError)
    {
        std::unique_lock<std::mutex> locker(loggerMutex);
        messages.push(Message(msg, isError));
        loggerCondition.notify_one();
    }

    static std::thread loggerThread(asyncLogger);
}

void error_log(const char* msg)
{
    logging::logSync(msg, true);
}

void debug_log(const char* msg)
{
    logging::logAsync(msg, false);
}

#endif
