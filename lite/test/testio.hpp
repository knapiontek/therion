
#ifndef TEST_IO_HPP
#define TEST_IO_HPP

#include "test/cs.hpp"
#include "core/io/file.hpp"
#include "core/io/inetstream.hpp"
#include "core/thread/event.hpp"
#include "core/thread/thread.hpp"
#include "core/env/os.hpp"
#include "core/env/console.hpp"

const core::uint16 the_port = 8071;

inline void test_file()
{
	core::String message = cs_host;
	io::File file(cs_filetest_tmp);

	// assemble filename
	core::String filename = core::Format(cs_file_format)
		.arg(file.path())
		.arg(file.core())
		.arg(file.ext())
		.end();
	core::verify(filename.equal(cs_filetest_tmp));
	filename = file.path()
		.append(cs_slash)
		.append(file.local());
	core::verify(filename.equal(cs_filetest_tmp));

	// write
	file.name().append(cs_tmp);
	file.open(io::File::read_write);
	io::Encode encode = file.output();
	encode.write_st(cs_outgoing);
	file.close();

	// exist
	core::verify(file.status(io::File::exist));

	// read
	file.open(io::File::read_only);
	io::Decode decode = file.input();
	decode.read_st(message);
	core::verify(message.equal(cs_outgoing));
	file.close();

	// status
	core::verify(file.status(io::File::exist | io::File::readable | io::File::writable | io::File::executable));

	// rename, remove
	core::verify(file.rename(core::String(file.name()).append(cs_tmp)));
	core::verify(file.remove());
}

inline void test_inetaddress()
{
	core::String host;
	core::uint16 port = 0;

	// inet address
	io::InetAddress address;
	core::verify(address.from_ip_format(cs_host, the_port));

	core::verify(address.to_ip_format(host, port));
	core::verify(cs_host.equal(host));
	core::verify(the_port == port);

	core::verify(address.to_hostname(host, port));
	core::verify(the_port == port);

	// remote host addresses by name
	core::Seq<io::InetAddress> address_seq;
	io::InetAddress::lookup(address_seq, cs_www_google_com, cs_http);
	core::Seq<io::InetAddress>::Iterator address_it = address_seq.head();
	while(address_it.next())
	{
		io::InetAddress& address = address_it.value();
		core::verify(address.to_ip_format(host, port));
	}

	// local inet interfaces
	core::Seq<io::InetLocal> local_seq;
	io::InetLocal::inet_info(local_seq);
	core::Seq<io::InetLocal>::Iterator local_it = local_seq.head();
	while(local_it.next())
	{
		io::InetLocal& local = local_it.value();
		core::verify(local.address().to_ip_format(host, port));
		core::verify(local.status(io::InetLocal::run | io::InetLocal::lo));
	}
}

class ServerThread : public thread::Thread
{
public:
	ServerThread() : thread::Thread(cs_server_thread)
	{

	}
	void interrupt()
	{
		the_server.close();
	}
	void bind(io::InetAddress& address)
	{
		the_server.bind(address);
	}
private:
	void run()
	{
		try
		{
			io::InetAddress client_address;
			while(the_server.listen(the_client, client_address))
			{
				io::Decode decode = the_client.input();
				io::Encode encode = the_client.output();

				decode.read_st(the_message);
				core::verify(the_message.equal(cs_outgoing));
				encode.write_st(cs_incoming);

				the_client.close();
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
	io::Inet the_client;
	io::InetServer the_server;
};

class MultiServerThread : public thread::Thread
{
public:
	MultiServerThread() : thread::Thread(cs_listen_thread)
	{

	}
	void close()
	{
		the_event.close();
		the_client_seq.erase_all();
		the_server_seq.erase_all();
	}
	void bind(core::Seq<io::InetAddress>& address_seq)
	{
		the_event.init();
		core::Seq<io::InetAddress>::Iterator it = address_seq.head();
		while(it.next())
		{
			io::InetAddress& address = it.value();
			io::InetServer& server = the_server_seq.append();
			server.bind(address);
			the_event.attach(server);
		}
	}
	void interrupt()
	{
		the_event.interrupt();
	}
private:
	void run()
	{
		try
		{
			bool run = true;
			while(run)
			{
				io::Inet::share inet;
				switch(the_event.wait(inet))
				{
				case io::InetEvent::interrupted:
					run = false;
					break;
				case io::InetEvent::connect:
					{
						io::InetServer& server = (io::InetServer&)(io::Inet&)inet;
						io::Inet& client = the_client_seq.append();
						server.accept(client);
						the_event.attach(client);
					}
					break;
				case io::InetEvent::disconnect:
					inet->close();
					break;
				case io::InetEvent::read:
					if(inet->can_read())
					{
						io::Decode decode = inet->input();
						io::Encode encode = inet->output();

						decode.read_st(the_message);
						core::verify(the_message.equal(cs_outgoing));
						encode.write_st(cs_incoming);
					}
					break;
				}
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
	io::InetEvent the_event;
	core::Seq<io::Inet> the_client_seq;
	core::Seq<io::InetServer> the_server_seq;
};

inline bool client_connect(io::InetAddress& address)
{
	core::String the_message;
	io::InetClient client;
	if(client.try_connect(address))
	{
		io::Decode decode = client.input();
		io::Encode encode = client.output();

		encode.write_st(cs_outgoing);
		decode.read_st(the_message);
		core::verify(the_message.equal(cs_incoming));

		return true;
	}
	return false;
}

inline void test_inet_server()
{
	io::InetAddress loopback_ipv4;
	io::InetAddress loopback_ipv6;
	loopback_ipv4.loopback_ipv4(the_port);
	loopback_ipv6.loopback_ipv6(the_port);

	io::InetAddress address;
	ServerThread server;

	// ipv4
	address.any_ipv4(the_port);
	server.bind(address);
	server.start();

	core::verify(client_connect(loopback_ipv4));
	core::verify(!client_connect(loopback_ipv6));

	server.interrupt();
	server.join();

	// ipv6
	address.any_ipv6(the_port);
	server.bind(address);
	server.start();

	core::verify(client_connect(loopback_ipv4));
	core::verify(client_connect(loopback_ipv6));

	server.interrupt();
	server.join();
}

inline void test_inet_multiserver()
{
	io::InetAddress loopback_ipv4;
	io::InetAddress loopback_ipv6;
	loopback_ipv4.loopback_ipv4(the_port);
	loopback_ipv6.loopback_ipv6(the_port);

	core::Seq<io::InetAddress> address_seq;
	MultiServerThread server;

	// ipv4
	io::InetAddress& any_ipv4 = address_seq.append();
	any_ipv4.any_ipv4(the_port);
	server.bind(address_seq);
	server.start();

	core::verify(client_connect(loopback_ipv4));
	core::verify(!client_connect(loopback_ipv6));

	server.interrupt();
	server.join();
	server.close();

	// ipv6
	io::InetAddress& any_ipv6 = address_seq.append();
	any_ipv4.any_ipv4(1 + the_port);
	any_ipv6.any_ipv6(the_port);
	server.bind(address_seq);
	server.start();

	core::verify(client_connect(loopback_ipv4));
	core::verify(client_connect(loopback_ipv6));

	server.interrupt();
	server.join();
}

inline void test_io()
{
	test_file();
	test_inetaddress();
	test_inet_server();
	test_inet_multiserver();
}

#endif // TEST_IO_HPP
