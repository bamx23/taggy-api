#ifndef PTREE_FIX_HPP_INCLUDED
#define PTREE_FIX_HPP_INCLUDED

#include "json_parser.hpp"

template <typename T>
struct StringTranslator
{
    typedef T internal_type;
    typedef T external_type;

    boost::optional<T> get_value(const T &v) { return  v.substr(1, v.size() - 2) ; }
    boost::optional<T> put_value(const T &v) { return '"' + v + '"'; }
};

void put_str(boost::property_tree::ptree &pt, const std::string &key, const std::string &str) {
    pt.put<std::string>(key, str, StringTranslator<std::string>());
}

#endif
