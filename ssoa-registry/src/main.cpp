/**
 *  main.cpp
 */

#include <registryimpl.h>
#include <ssoa/registry/registrylistener.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <ssoa/logger.h>
#include <ssoa/utils.h>

using namespace std;
using namespace ssoa;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    string address, port;
    int num_threads;

    po::options_description description("Allowed options");
    description.add_options()
        ("help,h", "Shows this help")
        ("address,a", po::value<string>(&address)->default_value("127.0.0.1"),
            "Listens on the given local address")
        ("port,p", po::value<string>(&port),
            "Listens on the given local port")
        ("threads,n", po::value<int>(&num_threads)->default_value(10),
            "Specifies the number of threads in the pool")
        ("log-marker,l", po::value<string>(&Logger::marker),
            "Specifies a string printed at the beginning of every log message");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, description), vm);
        po::notify(vm);
    }
    catch (const exception& e) {
        cerr << "Error parsing command line arguments: " << e.what() << endl;
        cerr << description;
        return EXIT_FAILURE;
    }

    if (vm.find("help") != vm.end()) {
        cout << description;
        return EXIT_FAILURE;
    }

    if (vm.find("port") == vm.end()) {
        cerr << "Local port not specified!" << endl;
        return EXIT_FAILURE;
    }

    try {
        // Initialize the library
        ssoa::setup();

        // Create an instance of the registry.
        RegistryImpl registry;

        // Initialize and run the server until stopped.
        ssoa::RegistryListener server(address, port, num_threads, registry);
        server.run();
    }
    catch (const exception& e) {
        Logger::error("Caught an exception: %1%", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
