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

#ifdef NEON_SERVER

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "network/Server.hpp"
#include "utils/Types.hpp"
#include "network/ServerDiscovery.hpp"
#include "utils/Args.hpp"
#include <iostream>
#include <thread>
#include <signal.h>
#include <atomic>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
std::atomic<bool> running(true);

///////////////////////////////////////////////////////////////////////////////
void signalHandler(int signum)
{
    std::cout << "\nShutdown signal received (" << signum << ")." << std::endl;
    running = false;
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    std::cout << "Starting game server..." << std::endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    tkd::Uint16 port = 55001;

    tkd::Args::addHandler("--port",
    [&port](const std::string& value)
    {
        if (value.empty()) {
            std::cerr << "Invalid port" << std::endl;
            return;
        }
        try {
            port = std::stoi(value);
        } catch (const std::exception& e) {
            std::cerr << "Unable to process port: " << e.what() << std::endl;
        }
    });

    tkd::Args::handleArgs(argc, argv);

    try {
        tkd::Server server(port);

        tkd::ServerDiscovery discovery(port);
        discovery.startBroadcasting();

        std::thread sthread([&server](void){
            try {
                while (running) {
                    server.update();
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
            } catch (const std::exception& e) {
                std::cerr << "Server thread error: " << e.what() << std::endl;
                running = false;
            }
        });

        while (running)
            server.run();
        std::cout << "Shutting down server..." << std::endl;
        if (sthread.joinable())
            sthread.join();
        std::cout << "Server shutdown complete." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

#else

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "utils/Types.hpp"
#include "core/Engine.hpp"
#include "utils/Args.hpp"
#include "utils/Macros.hpp"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    bool debug = false;

    tkd::Args::addHandler("--debug",
    [&debug](const std::string& value)
    {
        IGNORE(value);
        debug = true;
    }, "Start the game in debug mode");

    tkd::Args::handleArgs(argc, argv);

    {
        tkd::Engine engine(debug);
        engine.start();
    }

    return (EXIT_SUCCESS);
}

#endif
