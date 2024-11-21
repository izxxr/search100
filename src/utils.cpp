/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */


#ifndef _S100_UTILS
#define _S100_UTILS

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <sys/stat.h>

/**
 * @brief Checks whether `str` ends with `substr`
 * 
 * @param  data:  the string to check ending of.
 * @param  substr:  the ending to match with.
 * 
 * @return bool - true if ending matched.
 */
bool stringEndsWith(std::string data, std::string substr)
{
    int diff = data.length() - substr.length();
    if (diff < 0)
        return false;

    return (data.substr(diff) == substr);
}

/**
 * @brief Converts the given string to lowercase.
 * 
 * @param data: the string to convert.
 * 
 * @return string - lowercase string.
 */
std::string stringToLower(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c){ return std::tolower(c); });
    return data;
}

/**
 * @brief Checks whether a file exists.
 * 
 * @param name: the name of file.
 * 
 * @return bool - true if file exists.
 */
bool checkFileExists (const std::string &name) {
  struct stat buffer;   
  return (stat(name.c_str(), &buffer) == 0); 
}

/**
 * @brief Logs a message in console.
 * 
 * @param msg: the message to output.
 * @param scope: the scope of log message (default e.g. INFO)
 * @param add_prefix: whether to add Search100 prefix (default: true)
 * @param indent: The level of indentation to add. (default: 0, no indentation)
 * 
 */
void log(
    std::string msg,
    std::string scope = "INFO",
    bool add_prefix = true,
    int indent = 0
)
{
    std::string prefix = "";
    if (indent)
        prefix.replace(0, indent - 1, "\t");
    if (add_prefix)
        prefix += "[Search100] ";

    if (!scope.length())
        std::cout << prefix;
    else
        std::cout << prefix << "[" << scope << "] ";

    std::cout << msg << std::endl;
}

#endif