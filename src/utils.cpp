/* Copyright (C) Izhar Ahmad & Mustafa Hussain, 2024-2025 */

#include <algorithm>
#include <cctype>
#include <string>

/**
 * @brief Checks whether `str` ends with `substr`
 * 
 * @param  data:  the string to check ending of.
 * @param  substr:  the ending to match with.
 * 
 * @return bool
 */
bool stringEndsWith(std::string data, std::string substr)
{
    return (data.substr(data.length() - substr.length()) == substr);
}

/**
 * @brief Converts the given string to lowercase.
 * 
 * @param data: the string to convert.
 * 
 * @return string
 */
std::string stringToLower(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return std::tolower(c); });
    return data;
}
