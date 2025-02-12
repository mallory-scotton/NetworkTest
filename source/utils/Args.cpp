///////////////////////////////////////////////////////////////////////////////
///
/// MIT License
///
/// Copyright(c) 2025 TekyoDrift
///
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following coditions:
///
/// The above copyright notice and this permission notice shall be included
/// in all copies or substantial portions of the Software?
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Args.hpp"
#include <iostream>
#include <exception>
#include <iomanip>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::map<
    std::string,
    std::pair<
        std::function<void(const std::string&)>,
        std::string
    >
> Args::m_handlers;

///////////////////////////////////////////////////////////////////////////////
std::mutex Args::m_mutex;

///////////////////////////////////////////////////////////////////////////////
std::function<
    void(const std::string&)
> Args::m_unknownArgHandler = [](const std::string& arg)
{
    std::cerr << "Unknown argument: " << arg << std::endl;
};

///////////////////////////////////////////////////////////////////////////////
bool Args::addHandler(
    const std::string& arg,
    const std::function<void(const std::string&)>& callback,
    const std::string& description
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_handlers.count(arg) > 0)
        return (false);
    m_handlers[arg] = std::make_pair(callback, description);
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
bool Args::addHandler(
    const std::string& arg,
    const std::string& shortArg,
    const std::function<void(const std::string&)>& callback, 
    const std::string& description
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_handlers.count(arg) > 0 || m_handlers.count(shortArg) > 0)
        return (false);
    m_handlers[arg] = std::make_pair(callback, description);
    m_handlers[shortArg] = std::make_pair(callback, description);
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void Args::handleArgs(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        std::string value;

        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos) {
            value = arg.substr(equalsPos + 1);
            arg = arg.substr(0, equalsPos);
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_handlers.count(arg) == 1) m_handlers[arg].first(value);
        else if (arg == "--help")     { printHelp(); exit(EXIT_SUCCESS); }
        else                            m_unknownArgHandler(arg);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Args::printHelp(void)
{
    std::cout << "Available commands:" << std::endl;

    size_t max_arg_length = 0;
    for (const auto& entry : m_handlers)
        max_arg_length = std::max(max_arg_length, entry.first.length());

    for (const auto& handler : m_handlers)
        std::cout << "  "
                  << std::left << std::setw(max_arg_length + 4)
                  << handler.first
                  << handler.second.second
                  << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
bool Args::hasHandler(const std::string& arg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return (m_handlers.count(arg) > 0);
}

///////////////////////////////////////////////////////////////////////////////
void Args::setUnknownArgHandler(
    std::function<void(const std::string&)> callback
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_unknownArgHandler = callback;
}

} // namespace tkd
