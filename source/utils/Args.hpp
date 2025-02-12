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
// Pragma
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <string>
#include <map>
#include <functional>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A flexible command-line argument handling class
///
/// This class provides a simple mechanism for registering and processing
/// command-line arguments with associated callback functions.
///
///////////////////////////////////////////////////////////////////////////////
class Args
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private properties
    ///////////////////////////////////////////////////////////////////////////
    static std::map<
        std::string,
        std::pair<
            std::function<void(const std::string&)>,
            std::string
        >
    > m_handlers;               //<! Storage for argument handlers
    static std::function<
        void(const std::string&)
    > m_unknownArgHandler;      //<! Handler for unrecognized arguments
    static std::mutex m_mutex;  //<! Mutex for multi-threading

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Register a new command-line argument handler
    ///
    /// \param arg The argument string to handle (e.g., "-v", "--help")
    /// \param callback The function to call when the argument is encountered
    /// \param description Optional description of the argument's purpose
    /// \param shortArg Optional short version of the arg
    ///
    /// \return True if the handler was added successfully, false if the
    /// argument already exists
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool addHandler(
        const std::string& arg,
        const std::function<void(const std::string&)>& callback,
        const std::string& description = ""
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Register a new command-line argument handler
    ///
    /// \param arg The argument string to handle (e.g., "--verbose", "--help")
    /// \param shortArg Short version of the arg (e.g, "-v", "-h")
    /// \param callback The function to call when the argument is encountered
    /// \param description Optional description of the argument's purpose
    ///
    /// \return True if the handler was added successfully, false if the
    /// argument already exists
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool addHandler(
        const std::string& arg,
        const std::string& shortArg,
        const std::function<void(const std::string&)>& callback,
        const std::string& description = ""
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Process all command-line arguments
    ///
    /// Iterates through arguments and calls corresponding handlers
    ///
    /// \param argc Number of command-line arguments
    /// \param argv Array of command-line argument strings
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void handleArgs(int argc, char *argv[]);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Print help information for registered arguments
    ///
    /// Displays all registered arguments and their descriptions
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void printHelp(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if a specific argument is registered
    ///
    /// \param arg The argument to check
    ///
    /// \return True if the argument is registered, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool hasHandler(const std::string& arg);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set a default handler for unknown arguments
    ///
    /// \param callback The function to call for unrecognized arguments
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void setUnknownArgHandler(
        std::function<void(const std::string&)> callback
    );
};

} // namespace tkd
