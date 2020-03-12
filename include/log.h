#pragma once

#include <iostream>

namespace sway
{

template <std::ostream *os>
class _logger
{
    template <class Arg, class... Args>
    void _print(Arg&& arg, Args&&... args)
    {
        (*os) << std::forward<Arg>(arg);
        using expander = int[];
        (void)expander{0, (void((*os) << std::forward<Args>(args)), 0)...};
        (*os) << std::endl;
    }

  public:
    template <class... Args>
    void info(Args&&... args) // info that might be useful with a verbose flag
    {
        (*os) << ("\033[1;32m[Info]\033[0m  ");
        _print(args...);
    }
    template <class... Args>
    void warning(Args&&... args) // non-fatal warnings
    {
        (*os) << ("\033[1;33m[Warning]\033[0m  ");
        _print(args...);
    }
    template <class... Args>
    void error(Args&&... args) // non-fatal errors or errors that are handled
    {
        (*os) << ("\033[1;31m[Error]\033[0m  ");
        _print(args...);
    }
    template <class... Args>
    void fatal_error(Args&&... args) // fatal errors, application will likely abort without an error message if this is ever printed
    {
        (*os) << ("\033[1;35m[Fatal Error]\033[0m  ");
        _print(args...);
    }
};

static _logger<&std::cout> log = {};


} // namespace sway