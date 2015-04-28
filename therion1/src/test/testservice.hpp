
#ifndef TEST_SERVICE_HPP
#define TEST_SERVICE_HPP

#include "test/cs.hpp"
#include "service/solver.hpp"
#include "core/env/console.hpp"
#include "core/thread/threadpool.hpp"

class House : public thread::Runnable
{
public:
	typedef core::Managed<House> manage;
public:
	~House()
	{

	}
	void run()
	{

	}
	void interrupt()
	{

	}
};

class Factory : public thread::Runnable
{
public:
	typedef core::Managed<Factory> manage;
public:
	~Factory()
	{

	}
	void run()
	{

	}
	void interrupt()
	{

	}
};

class ServiceThread : public thread::Thread
{
public:
	ServiceThread() : thread::Thread(cs_service_thread),
		the_house_list(0xF), the_factory_list(0xF)
	{

	}
	void close()
	{
		the_service.close();
		the_house_list.erase_all();
		the_factory_list.erase_all();
	}
	void start()
	{
		the_service.init();

		io::InetLocal::any(the_therion_address, 8080);
		the_service.attach(the_therion_address, the_therion_transport);

		io::InetLocal::any(the_http_address, 8081);
		the_service.attach(the_http_address, the_http_transport);

		thread::Thread::start();
	}
	void interrupt()
	{
		the_service.interrupt();
		the_thread_pool.interrupt();
		thread::Thread::interrupt();
	}
	void join()
	{
		the_thread_pool.join();
		thread::Thread::join();
	}
private:
	void run()
	{
		try
		{
			service::ServiceClient::share client;
			while(the_service.wait(client))
			{
				if(core::nil(client->runnable))
				{
					core::String target = client->transport->read_target(client);
					if(cs_factory_target.equal(target))
						client->runnable = the_factory_list.append();
					else if(cs_house_target.equal(target))
						client->runnable = the_house_list.append();
					else
						env::Fail(cs_target_not_exist).arg(target).fire();
				}
				the_thread_pool.start(client->runnable);
			}
		}
		catch(env::Exception& exception)
		{
			env::Con::exception(exception);
		}
		catch(...)
		{
			core::verify(false);
		}
	}
private:
	core::String the_message;
	thread::ThreadPool the_thread_pool;
	service::Service the_service;
	service::TherionTransport the_therion_transport;
	service::HttpTransport the_http_transport;
	io::InetAddress the_therion_address;
	io::InetAddress the_http_address;
	core::List<House> the_house_list;
	core::List<Factory> the_factory_list;
};

inline void test_service()
{
	ServiceThread service_thread;
	service_thread.start();

	service_thread.interrupt();
	service_thread.join();
	service_thread.close();
}

#endif // TEST_SERVICE_HPP
