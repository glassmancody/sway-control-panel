#pragma once 

#include "config.h"

#include <string>
#include <algorithm>
#include <locale>
#include <memory>

namespace sway {

static bool file_exists(const string& path)
{
    return Glib::file_test(path, Glib::FILE_TEST_EXISTS);
}

static bool dir_exists(const string& path)
{
    return Glib::file_test(path, Glib::FILE_TEST_IS_DIR);
}

static inline std::string resource(const std::string &path)
{
  return app.resource_root + path;
}

static void ltrim_whitespace(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

static void rtrim_whitespace(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

} // namespace sway 
