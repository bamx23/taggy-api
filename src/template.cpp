#include "template.hpp"
#include <fastcgi++/fcgistream.hpp>

std::stringstream &operator<<(std::stringstream &out, const Template &self)
{
    self.header(out);
    out << self.str();
    self.footer(out);
    return out;
}

template <class T>
void dumpTo(T &out, const Template &self) { }

template <>
void dumpTo<Fastcgipp::Fcgistream<wchar_t> >(Fastcgipp::Fcgistream<wchar_t> &out, Template const &self)
{
    std::stringstream temp;
    temp << self;
    out.dump(temp);
}
