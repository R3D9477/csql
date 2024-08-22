/** \file */

#pragma once

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace csql
{
namespace core
{

template <typename T> typename std::enable_if<std::is_fundamental<T>::value, std::string>::type stringify(const T &t)
{
    return std::to_string(t);
}

template <typename T> typename std::enable_if<!std::is_fundamental<T>::value, std::string>::type stringify(const T &t)
{
    return std::string(t);
}

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type quoted_stringify(const T &t)
{
    return "'" + stringify(t) + "'";
}

template <typename T>
typename std::enable_if<!std::is_same<T, std::string>::value, std::string>::type quoted_stringify(const T &t)
{
    return stringify(t);
}

template <typename T>
typename std::enable_if<std::is_same<T, const char *>::value, std::string>::type stdstr_or_type(const T t)
{
    return std::string{static_cast<const char *>(t)};
}

template <typename T> typename std::enable_if<!std::is_same<T, const char *>::value, T>::type stdstr_or_type(const T t)
{
    return t;
}

[[maybe_unused]] static std::string removeSqlDelimiter(const std::string &expr_)
{
    return expr_.empty() ? expr_ : expr_[expr_.size() - 1] == ';' ? expr_.substr(0, expr_.size() - 1) : expr_;
}

[[maybe_unused]] static std::stringstream &pop_back(std::stringstream &stream)
{
    if (stream.tellp() > 0)
        stream.str(stream.str().erase(static_cast<std::size_t>(stream.tellp()) - 1));
    return stream;
}

[[maybe_unused]] static std::ostringstream &pop_back(std::ostringstream &stream)
{
    if (stream.tellp() > 0)
        stream.str(stream.str().erase(static_cast<std::size_t>(stream.tellp()) - 1));
    return stream;
}

[[maybe_unused]] static std::string &ltrim(std::string &s)
{
    auto it = std::find_if(s.begin(), s.end(), [](char c) { return !std::isspace<char>(c, std::locale::classic()); });
    s.erase(s.begin(), it);
    return s;
}

[[maybe_unused]] static std::string &rtrim(std::string &s)
{
    auto it = std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace<char>(c, std::locale::classic()); });
    s.erase(it.base(), s.end());
    return s;
}

[[maybe_unused]] static std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

} // namespace core
} // namespace csql
