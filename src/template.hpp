#include <sstream>

class Template : public std::stringstream
{
protected:
    virtual void header(std::stringstream &out) const { }
    virtual void footer(std::stringstream &out) const { }
public:
    friend std::stringstream &operator<<(std::stringstream &out, const Template &self);
};

template <class T>
void dumpTo(T &out, const Template &self);
