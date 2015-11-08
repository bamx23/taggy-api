#ifndef JSON_HPP
#define JSON_HPP

#include "template.hpp"

class JSONAbstract
{
protected:
    tstream stream;
public:
    virtual std::wstring str() const { return stream.str(); }

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
    std::wstring escapeJsonString(const std::wstring& input) {
        std::wostringstream ss;
        for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
            switch (*iter) {
                case '\\': ss << "\\\\"; break;
                case '"': ss << "\\\""; break;
                case '/': ss << "\\/"; break;
                case '\b': ss << "\\b"; break;
                case '\f': ss << "\\f"; break;
                case '\n': ss << "\\n"; break;
                case '\r': ss << "\\r"; break;
                case '\t': ss << "\\t"; break;
                default: ss << *iter; break;
            }
        }
        return ss.str();
    }
public:
    JSONVar() { }

    JSONVar(const wchar_t *str) : JSONVar(std::wstring(str)) { }
    JSONVar(const std::wstring &str) { stream << "\"" << escapeJsonString(str) << "\""; }

    JSONVar(int x) { stream << x; }
    JSONVar(long x) { stream << x; }

    JSONVar(float x) { stream << std::setprecision(7) << std::fixed << x; }
    JSONVar(double x) { stream << std::setprecision(7) << std::fixed << x; }
};

class JSONKeyValue : public JSONAbstract
{
public:
    JSONKeyValue(const std::wstring &key, const JSONVar &value)
    {
        stream << "\"" << key << "\":" << value.str();
    }
};

class JSONArray : public JSONVar
{
    size_t count;
public:
    JSONArray() : count(0) { }

    virtual std::wstring str() const { return L"[" + stream.str() + L"]"; } 
    
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

    virtual std::wstring str() const { return L"{" + stream.str() + L"}"; }

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
