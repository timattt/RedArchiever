#pragma once

#include <iostream>
#include <vector>
#include <tuple>
#include <map>

template <typename T>
std::ostream&
operator << (std::ostream& os,
             const std::vector <T>& vec)
{
    auto begin = vec.cbegin ();
    const auto end = vec.cend ();
    if (begin == end) {
        return os;
    }
    
    os << *begin++;

    while (begin != end) {
        os << ", " << *begin++;
    }

    return os;
} // operator << (std::ostream& os, const std::vector <T>& vec)

template <typename F, typename S>
std::ostream&
operator << (std::ostream& os,
             const std::pair <F, S>& pair)
{
    return os << pair.first << ": " << pair.second;
} // operator << (std::ostream& os, const std::pair <F, S>& pair)

template <typename K, typename V>
std::ostream&
operator << (std::ostream& os,
             const std::map <K, V>& map)
{
    os << "{";

    const std::size_t size = map.size ();
    auto iter = map.cbegin ();
    for (std::size_t i = 0; i + 1 < size; ++i) {
        os << *iter++ << ", ";
    }

    return  os << *iter << "}";
} // operator << (std::ostream& os, const std::map <K, V>& map)

template <typename... Args>
std::ostream&
print_all (std::ostream& os,
           const Args&... args)
{
    return (os << ... << args);
} // print_all (std::ostream& os, const Args&... args)

namespace detail {

template <typename T, typename SepT = std::string>
class AddSeparator {
    const T& ref_;
    const SepT& sep_;

public:
    AddSeparator (const T& ref, SepT sep = " "):
        ref_ (ref),
        sep_ (sep)
    {} // AddSeparator (const T& ref, SepT sep)

    std::ostream& print (std::ostream& os) const {
        return os << sep_ << ref_;
    } // print (std::ostream& os) const
};

} // namespace detail

template <typename T>
std::ostream&
operator << (std::ostream& os,
             const detail::AddSeparator <T>& val)
{
    return val.print (os);
} // operator << (std::ostream& os, const detail::AddSeparator <T>& val)

template <typename Arg, typename... Args>
std::ostream&
print_all_sep (std::string sep,
               std::ostream& os,
               const Arg& arg,
               const Args&... args)
{
    return ((os << arg) << ... << detail::AddSeparator (args, sep));
} // print_all_sep (std::string sep, std::ostream& os, const Arg& arg, const Args&... args)

template <typename Arg, typename... Args>
std::ostream&
print_all_sep (std::ostream& os,
               const Arg& arg,
               const Args&... args)
{
    return print_all_sep (" ", os, arg, args...);
} // print_all_sep (std::ostream& os, const Arg& arg, const Args&... args)

template <typename... Args>
std::ostream&
operator << (std::ostream& os,
             const std::tuple <Args...>& tuple)
{
    os << "{";
    std::apply (
        [&os] (const auto&... values) {
            print_all_sep (", ", os, values...);
        },
        tuple
    );
    os << "}";

    return os;
} // operator << (std::ostream& os, const std::tuple <Args...>& tuple)

template <typename ForwardIteratorBegin, typename ForwardIteratorEnd>
std::ostream&
print_sep (std::ostream& os,
           ForwardIteratorBegin begin,
           ForwardIteratorEnd end,
           const std::string& sep = " ")
{
    if (begin == end) {
        return os;
    }

    os << *begin++;

    for (; begin != end; ++begin) {
        os << sep << *begin;
    }

    return os;
}
