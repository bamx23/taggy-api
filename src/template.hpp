#include <sstream>
#include <fastcgi++/fcgistream.hpp>

typedef std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > tstream;
typedef Fastcgipp::Fcgistream<wchar_t> fcgistream;

class Template : public tstream
{
protected:
    virtual void header(tstream &out) const { }
    virtual void footer(tstream &out) const { }
public:
    friend tstream &operator<<(tstream &out, const Template &self);
    friend fcgistream &operator<<(fcgistream &out, const Template &self);
};
