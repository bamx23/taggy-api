#ifndef JSON_HPP
#define JSON_HPP

#include "template.hpp"

class JSONAbstract
{
protected:
    tstream stream;
public:
    virtual tstring str() const { return stream.str(); }

    friend tstream &operator<<(tstream &out, const JSONAbstract &self)
    {
        out << self.str();
        return out;
    }

    friend fcgistream &operator<<(fcgistream &out, const JSONAbstract &self)
    {
        out << self.str();
        return out;
    }
};

class JSONVar : public JSONAbstract
{
    tstring escapeJsonString(const tstring& input) {
        tstream buffer;
        for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
            switch (*iter) {
                case '\\': buffer << "\\\\"; break;
                case '"':  buffer << "\\\""; break;
                case '/':  buffer << "\\/"; break;
                case '\b': buffer << "\\b"; break;
                case '\f': buffer << "\\f"; break;
                case '\n': buffer << "\\n"; break;
                case '\r': buffer << "\\r"; break;
                case '\t': buffer << "\\t"; break;
                default: buffer << *iter; break;
            }
        }
        return buffer.str();
    }
public:
    JSONVar() { }

    JSONVar(const tchar_t *str) : JSONVar(tstring(str)) { }
    JSONVar(const tstring &str) { stream << "\"" << escapeJsonString(str) << "\""; }

    JSONVar(int x) { stream << x; }
    JSONVar(long x) { stream << x; }

    JSONVar(float x) { stream << std::setprecision(7) << std::fixed << x; }
    JSONVar(double x) { stream << std::setprecision(7) << std::fixed << x; }
};

class JSONKeyValue : public JSONAbstract
{
public:
    JSONKeyValue(const tstring &key, const JSONVar &value)
    {
        stream << "\"" << key << "\":" << value.str();
    }
};

class JSONArray : public JSONVar
{
    size_t count;
public:
    JSONArray() : count(0) { }

    virtual tstring str() const { return "[" + stream.str() + "]"; } 
    
    JSONArray &operator<<(const JSONVar &var)
    {
        tstream bufferStream;
        bufferStream << (count++ ? "," : ""); 
        bufferStream << var.str();
        stream << bufferStream.str();
        return *this;
    }
};

class JSONObject : public JSONVar
{
    size_t count;
public:
    JSONObject() : count(0) { }

    virtual tstring str() const { return "{" + stream.str() + "}"; }

    JSONObject &operator<<(const JSONKeyValue &keyValue)
    {
        tstream temp;
        temp << (count++ ? "," : ""); 
        temp << keyValue.str();
        stream << temp.str();
        return *this;
    }
};

typedef JSONKeyValue jkvp;
typedef JSONObject jobj;
typedef JSONArray jarr;

#endif
