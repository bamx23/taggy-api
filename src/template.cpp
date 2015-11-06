#include "template.hpp"

tstream &operator<<(tstream &out, const Template &self)
{
    self.header(out);
    out << self.str();
    self.footer(out);
    return out;
}

fcgistream &operator<<(fcgistream &out, const Template &self)
{
    tstream temp;
    temp << self;
    out << temp.str();
    return out;
}
