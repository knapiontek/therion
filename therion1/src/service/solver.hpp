
#ifndef SERVICE_SOLVER_HPP
#define SERVICE_SOLVER_HPP

#include "service/cs.hpp"
#include "service/http.hpp"
#include "core/core/list.hpp"

NAMESPACE_BEGIN(service);

struct ServiceServer : public io::InetServer
{
	typedef core::Shared<ServiceServer> share;
	Transport::share transport;
};

struct ServiceClient : public io::Inet
{
	typedef core::Shared<ServiceClient> share;
	Transport::share transport;
	thread::Runnable::share runnable;
};

class Service
{
public:
	Service() : the_server_list(0x9), the_client_list(0x80)
	{

	}
	void close()
	{
		the_event.close();
		the_server_list.erase_all();
		the_client_list.erase_all();
	}
	void init()
	{
		the_event.init();
	}
	void interrupt()
	{
		the_event.interrupt();
	}
	void attach(io::InetAddress& address, Transport& transport)
	{
		ServiceServer& server = the_server_list.append();
		server.bind(address);
		server.transport = transport;
		the_event.attach(server);
	}
	bool wait(ServiceClient::share& client)
	{
		while(true)
		{
			io::Inet::share inet;
			switch(the_event.wait(inet))
			{
			case io::InetEvent::interrupted:
				return false;
			case io::InetEvent::connect:
				{
					ServiceServer::share server = inet;
					ServiceClient& client = the_client_list.append();
					client.transport = server->transport;
					server->accept(client);
					the_event.attach(client);
				}
				break;
			case io::InetEvent::disconnect:
				inet->close();
				break;
			case io::InetEvent::read:
				if(inet->can_read())
				{
					client = inet;
					return true;
				}
				break;
			}
		}
	}
private:
	io::InetEvent the_event;
	core::List<ServiceServer> the_server_list;
	core::List<ServiceClient> the_client_list;
};

NAMESPACE_END(service);

#endif // SERVICE_SOLVER_HPP

