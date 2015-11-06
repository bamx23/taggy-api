#include "template.hpp"

std::stringstream &operator<<(std::stringstream &out, const Template &self)
{
    self.header(out);
    out << self.in.str();
    self.footer(out);
    return out;
}
