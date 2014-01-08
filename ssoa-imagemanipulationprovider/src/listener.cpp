/*
 * server.cpp
 */

#include <listener.h>
#include <ssoa/service/serviceskeleton.h>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using std::string;
using namespace boost::asio::ip;

namespace imagemanipulationprovider
{
    Listener::Listener(const string& host, const string& port, size_t thread_pool_size) :
        threadPoolSize(thread_pool_size), signals(ioService), acceptor(ioService)
    {
        signals.add(SIGINT);
        signals.add(SIGTERM);
#if defined(SIGQUIT)
        signals.add(SIGQUIT);
#endif
        signals.async_wait(boost::bind(&Listener::handleStop, this));

        tcp::resolver resolver(ioService);
        tcp::resolver::query query(host, port);
        tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor.open(endpoint.protocol());
        acceptor.set_option(tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();

        startAccept();
    }

    void Listener::run()
    {
        boost::thread_group threads;
        for (size_t i = 0; i < threadPoolSize; ++i) {
            threads.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
        }

        threads.join_all();
    }

    void Listener::startAccept()
    {
        clientSocket.reset(new tcp::socket(ioService));

        acceptor.async_accept(*clientSocket.get(), boost::bind(&Listener::handleAccept, this,
                                                               boost::asio::placeholders::error));
    }

    void Listener::handleAccept(const boost::system::error_code& e)
    {
        if (!e) {
            ServiceSkeleton::start(std::move(clientSocket));
        }

        startAccept();
    }

    void Listener::handleStop()
    {
        ioService.stop();
    }
}
