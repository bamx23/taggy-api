#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP



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
