#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#include <sstream>
#include <fastcgi++/fcgistream.hpp>

typedef char tchar_t;
typedef std::basic_string<tchar_t> tstring;
typedef std::basic_stringstream<tchar_t, std::char_traits<tchar_t>, std::allocator<tchar_t> > tstream;
typedef Fastcgipp::Fcgistream<tchar_t> fcgistream;

class Template : public tstream
{
protected:
    virtual void header(tstream &out) const { }
    virtual void footer(tstream &out) const { }
public:
    friend tstream &operator<<(tstream &out, const Template &self);
    friend fcgistream &operator<<(fcgistream &out, const Template &self);
};

#endif
