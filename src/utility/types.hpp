#ifndef TYPES_HPP
#define TYPES_HPP

typedef char tchar_t;
typedef std::basic_string<tchar_t> tstring;
typedef std::basic_stringstream<tchar_t, std::char_traits<tchar_t>, std::allocator<tchar_t> > tstream;
typedef Fastcgipp::Fcgistream<tchar_t> fcgistream;

#endif
