#include <sstream>

class Template
{
protected:
    virtual void header(std::stringstream &out) const { }
    virtual void footer(std::stringstream &out) const { }
public:
    std::stringstream in;

    friend std::stringstream &operator<<(std::stringstream &out, const Template &self);
};
