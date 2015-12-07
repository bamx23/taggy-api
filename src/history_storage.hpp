#ifndef HISTORY_STORAGE_HPP_INCLUDED
#define HISTORY_STORAGE_HPP_INCLUDED

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/conversion.hpp>

#include <sys/stat.h>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <string>
#include <fstream>

#include "error.hpp"
#include "ptree-fix.hpp"
#include "json_parser.hpp"

using namespace boost::posix_time;

namespace history
{
    static char const indexFilename[] = "data/index.dat";
    static char const historyFilename[] = "data/history.dat";

    static int32_t const nameSize = 3;
    typedef int64_t sizeType;
    typedef int32_t valueType;
    typedef int64_t timeType;

    static size_t const cacheSize = 1000;

    class Currency
    {
        std::map<std::string, float> rates;
        ptime time;
        boost::property_tree::ptree json;

        std::string utcDate(const boost::posix_time::ptime &time)
        {
            using namespace boost::posix_time;
            static std::locale loc(std::locale::classic(), new time_facet("%Y%m%dT%H%M%S"));

            std::ostringstream buffer;
            buffer.imbue(loc);
            buffer << time;
            return buffer.str();
        }

        void formatJson()
        {
            using boost::property_tree::ptree;

            ptree result;

            ptree currencyRates;
            for (auto &rate : rates) {
                ptree element;
                put_str(element, "name", rate.first);
                element.put<float>("value", rate.second);
                currencyRates.push_back(std::make_pair("", element));
            }
            result.add_child("currency", currencyRates);

            put_str(result, "time", utcDate(time));

            json = result;
        }

    public:
        Currency() { }

        Currency(const ptime &_time, const std::map<std::string, float> &_rates)
            :time(_time), rates(_rates)
        { }

        Currency(const Currency &other)
        {
            rates = other.rates;
            time = other.time;
        }

        void read(sizeType offset)
        {
            FILE *file = fopen(historyFilename, "r");
            fseek(file, offset, SEEK_SET);

            sizeType count;
            valueType value;
            timeType stime;
            char buf[nameSize + 1] = {0};

            fread(&stime, sizeof(timeType), 1, file);
            time = boost::posix_time::from_time_t(stime);

            rates.clear();
            fread(&count, sizeof(sizeType), 1, file);
            for (sizeType index = 0; index < count; ++index) {
                fread(buf, sizeof(char), nameSize, file);
                fread(&value, sizeof(valueType), 1, file);

                rates[std::string(buf)] = *((float *)&value);
            }

            fclose(file);

            formatJson();
        }

        sizeType append() const
        {
            FILE *file = fopen(historyFilename, "a");
            sizeType offset;
            fgetpos(file, &offset);

            ptime start(boost::gregorian::date(1970, 1, 1));
            sizeType count = (sizeType)rates.size(), value;
            timeType stime = (timeType)(time - start).total_seconds();

            fwrite(&stime, sizeof(timeType), 1, file);
            fwrite(&count, sizeof(sizeType), 1, file);
            for (auto &kvp : rates) {
                value = kvp.second;
                fwrite(kvp.first.data(), sizeof(char), nameSize, file);
                fwrite(&kvp.second, sizeof(valueType), 1, file);
            }

            fclose(file);

            return offset;
        }

        boost::property_tree::ptree getJson() const
        {
            return json;
        }
    };

    typedef Currency *pCurrency;

    class Index
    {
        std::map<ptime, size_t> offsets;

        inline bool fileExists(const std::string &name) {
            struct stat buffer;
            return (stat(name.c_str(), &buffer) == 0);
        }

        void read()
        {
            if (fileExists(indexFilename)) {
                FILE *fin = fopen(indexFilename, "r");
                timeType time;
                sizeType offset;

                while (!feof(fin)) {
                    fread(&time, sizeof(timeType), 1, fin);
                    fread(&offset, sizeof(sizeType), 1, fin);
                    offsets[boost::posix_time::from_time_t(time)] = (size_t)offset;
                }
                fclose(fin);
            }
        }

        void append(const ptime &time, const sizeType &offset) const
        {
            FILE *fout = fopen(indexFilename, "a");

            ptime start(boost::gregorian::date(1970, 1, 1));
            timeType stime = (timeType)(time - start).total_seconds();

            fwrite(&stime, sizeof(timeType), 1, fout);
            fwrite(&offset, sizeof(sizeType), 1, fout);

            fclose(fout);
        }

        void fix()
        {
            FILE *fin = fopen(indexFilename, "r");
            FILE *fout = fopen(indexFilename, "w");

            timeType time;
            sizeType offset, count;

            while (!feof(fin)) {
                fgetpos(fin, &offset);
                fread(&time, sizeof(timeType), 1, fin);
                fread(&count, sizeof(sizeType), 1, fin);

                offsets[boost::posix_time::from_time_t(time)] = (size_t)offset;

                fwrite(&time, sizeof(timeType), 1, fout);
                fwrite(&offset, sizeof(sizeType), 1, fout);

                fseek(fin, (sizeof(char) * nameSize + sizeof(valueType)) * count, SEEK_CUR);
            }

            fclose(fout);
            fclose(fin);
        }

    public:
        Index()
        {
            read();
        }

        bool containsOffset(const ptime &time) const
        {
            return offsets.find(time) != offsets.end();
        }

        sizeType getOffset(const ptime &time)
        {
            return offsets[time];
        }

        void addOffset(const ptime &time, const sizeType &offset)
        {
            offsets[time] = offset;
            append(time, offset);
        }
    };

    class Cache
    {
        Index index;
        std::map<ptime, pCurrency> cache;

        std::map<ptime, ptime> cacheCallsIndex;
        std::set< std::pair<ptime, ptime> > cacheCalls;

        void updateCacheCall(const ptime& time)
        {
            auto now = microsec_clock::universal_time();
            auto it = cacheCallsIndex.find(time);
            if (it != cacheCallsIndex.end()) {
                cacheCalls.erase(std::make_pair(it->second, time));
            }

            cacheCalls.insert(std::make_pair(now, time));
            cacheCallsIndex[time] = now;

            checkCache();
        }

        void checkCache()
        {
            if (cacheCalls.size() > cacheSize) {
                auto it = cacheCalls.begin();

                cacheCallsIndex.erase(it->second);

                auto kvp = cache.find(it->second);
                delete kvp->second;
                cache.erase(kvp);

                cacheCalls.erase(it);
            }
        }

    public:
        ~Cache()
        {
            for (auto &kvp : cache) {
                delete kvp.second;
            }
        }

        bool containsCurrency(const ptime &time) const
        {
            return index.containsOffset(time);
        }

        Currency &getCurrency(const ptime &time)
        {
            pCurrency cur;

            auto it = cache.find(time);
            if (it != cache.end()) {
                cur = it->second;
            } else {
                cur = new Currency();
                cur->read(index.getOffset(time));
                cache[time] = cur;
            }

            updateCacheCall(time);

            return *cur;
        }

        Currency &addCurrency(const ptime &time, const std::map<std::string, float> &rates)
        {
            pCurrency cur = new Currency(time, rates);
            auto offset = cur->append();
            index.addOffset(time, offset);
            cache[time] = cur;

            updateCacheCall(time);

            return *cur;
        }
    };

    class Storage
    {
        Cache cache;

        ptime roundedTime(const ptime &time)
        {
            time_duration tod = seconds(time.time_of_day().total_seconds());
            time_duration roundedDownTod = hours(tod.hours());
            ptime result(time.date(), roundedDownTod);
            return result;
        }

    public:
        Currency &getCurrency(const ptime &time)
        {
            return cache.getCurrency(roundedTime(time));
        }

        void addCurrency(const ptime &time, const std::map<std::string, float> &rates)
        {
            if (!cache.containsCurrency(roundedTime(time))) {
                cache.addCurrency(roundedTime(time), rates);
            } else {
                debug_log("Tried to add existing currency");
            }
        }
    };
}

static history::Storage staticHistoryStorage;

#endif
