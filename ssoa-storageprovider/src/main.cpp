/**
 * main.cpp
 */

#include <listener.h>
#include <storeimageserviceimpl.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <logger.h>
#include <registry/registry.h>

using namespace std;
using namespace storageprovider;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    string address, port;
    string registryAddress, registryPort;
    int num_threads;

    po::options_description description("Allowed options");
    description.add_options()
        ("help", "Shows this help")
        ("address,a", po::value<string>(&address), "Listens on the given local address")
        ("port,p", po::value<string>(&port), "Listens on the given local port")
        ("registry-address,A", po::value<string>(&registryAddress), "Specifies the address of the registry")
        ("registry-port,P", po::value<string>(&registryPort), "Specifies the port of the registry")
        ("threads,n", po::value<int>(&num_threads)->default_value(10), "Specifies the number of threads in the pool");

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
    if (vm.find("registry-address") == vm.end()) {
        cerr << "Registry address not specified!" << endl;
        return EXIT_FAILURE;
    }
    if (vm.find("registry-port") == vm.end()) {
        cerr << "Registry port not specified!" << endl;
        return EXIT_FAILURE;
    }
    if (vm.find("address") == vm.end()) {
        Logger::info() << "Local address not specified: server will listen on the loopback interface." << endl;
    }

    Registry::initialize(registryAddress, registryPort);
    Logger::info() << "Initialized registry as " << registryAddress << ":" << registryPort << "." << endl;

    try {
        Registry::registerService(string(StoreImageServiceImpl::serviceSignature()), address, port);
        Logger::info() << "Services registered on the registry." << endl;
    }
    catch (const exception& e) {
        Logger::error() << "Caught an exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;
    try {
        // Initialize and run the server until stopped.
        Listener server(address, port, num_threads);
        Logger::info() << "Server started." << endl;
        server.run();
    }
    catch (const exception& e) {
        Logger::error() << "Caught an exception: " << e.what() << endl;
        // Do not return: deregister service.
        status = EXIT_FAILURE;
    }

    try {
        Registry::deregisterService(string(StoreImageServiceImpl::serviceSignature()), address, port);
        Logger::info() << "Services deregistered from the registry." << endl;
    }
    catch (const exception& e) {
        Logger::error() << "Caught an exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return status;
}