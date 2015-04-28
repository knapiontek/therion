
#ifndef IO_INET_STREAM_HPP
#define IO_INET_STREAM_HPP

#include "core/io/cs.hpp"
#include "core/io/inetaddress.hpp"
#include "core/io/input.hpp"
#include "core/io/output.hpp"
#include "core/env/env.hpp"
#include <sys/eventfd.h>
#include <sys/epoll.h>

NAMESPACE_BEGIN(io);

class Inet
{
	friend class InetClient;
	friend class InetServer;
	friend class InetEvent;
public:
	typedef core::Shared<Inet> share;
public:
	Inet()
	{
		the_handle = -1;
		the_input.the_inet = this;
		the_output.the_inet = this;
	}
	~Inet()
	{
		close();
	}
	void close()
	{
		if(-1 != the_handle)
		{
			::shutdown(the_handle, SHUT_RDWR);
			::close(the_handle);
			the_handle = -1;
		}
	}
	bool can_read()
	{
		bool any;
		return (0 < ::recv(the_handle, &any, sizeof(any), MSG_PEEK | MSG_DONTWAIT));
	}
	Input& input()
	{
		return the_input;
	}
	Output& output()
	{
		return the_output;
	}
private:
	class InetInput : Input
	{
		friend class Inet;
		void input(core::uint8* data, core::uint32 size)
		{
			core::uint32 received = ::recv(the_inet->the_handle, data, size, MSG_WAITALL);
			if(received != size)
				env::Fail(cs_receive_data_fail).arg(size).fire();
		}
		Inet* the_inet;
	};
	class InetOutput : Output
	{
		friend class Inet;
		void output(core::uint8* data, core::uint32 size)
		{
			core::uint32 sent = ::send(the_inet->the_handle, data, size, MSG_NOSIGNAL);
			if(sent != size)
				env::Fail(cs_send_data_fail).arg(size).fire();
		}
		Inet* the_inet;
	};
private:
	core::int32 the_handle;
	InetInput the_input;
	InetOutput the_output;
};

class InetClient : public Inet
{
public:
	bool try_connect(InetAddress& address)
	{
		core::assert(-1 == the_handle);
		the_handle = ::socket(address.the_sockaddr.sa_family, SOCK_STREAM, 0);
		if(-1 == the_handle)
			return false;
		core::int32 on = 1;
		if(!::setsockopt(the_handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)))
			return !::connect(the_handle, &address.the_sockaddr, sizeof(sockaddr_storage));
		return false;
	}
	void connect(InetAddress& address)
	{
		if(!try_connect(address))
		{
			core::String host;
			core::uint16 port;
			if(address.to_ip_format(host, port))
				env::Fail(cs_connect_fail).arg(host).arg(port).fire();
			else
				env::Fail::fire(cs_connect_address_fail);
		}
	}
};

class InetServer : public Inet
{
public:
	bool try_bind(InetAddress& address)
	{
		core::assert(-1 == the_handle);
		the_handle = ::socket(address.the_sockaddr.sa_family, SOCK_STREAM, 0);
		if(-1 == the_handle)
			return false;
		core::int32 on = 1;
		if(::setsockopt(the_handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)))
			return false;
		if(::bind(the_handle, &address.the_sockaddr, sizeof(sockaddr_storage)))
			return false;
		return !::listen(the_handle, 0);
	}
	void bind(InetAddress& address)
	{
		if(!try_bind(address))
		{
			core::String host;
			core::uint16 port = 0;
			address.to_ip_format(host, port);
			env::Fail(cs_bind_address_fail).arg(host).arg(port).fire();
		}
	}
	void accept(Inet& client)
	{
		core::assert(-1 == client.the_handle);
		client.the_handle = ::accept(the_handle, 0, 0);
		if(-1 == client.the_handle)
			env::Fail::fire(cs_accept_fail);
	}
	void accept(Inet& client, InetAddress& address)
	{
		core::assert(-1 == client.the_handle);
		core::uint32 size = 0;
		client.the_handle = ::accept(the_handle, &address.the_sockaddr, &size);
		if(-1 == client.the_handle)
			env::Fail::fire(cs_accept_fail);
	}
	bool listen(Inet& client)
	{
		core::assert(-1 == client.the_handle);
		client.the_handle = ::accept(the_handle, 0, 0);
		return (-1 != client.the_handle);
	}
	bool listen(Inet& client, InetAddress& address)
	{
		core::assert(-1 == client.the_handle);
		core::uint32 size = 0;
		client.the_handle = ::accept(the_handle, &address.the_sockaddr, &size);
		return (-1 != client.the_handle);
	}
};

class InetEvent
{
public:
	enum Type
	{
		interrupted,
		connect,
		disconnect,
		read
	};
public:
	InetEvent()
	{
		the_handle = -1;
		the_interrupt_handle = -1;
	}
	~InetEvent()
	{
		close();
	}
	void init()
	{
		// ready events
		the_last = 0;

		// create epoll
		core::assert(-1 == the_handle);
		the_handle = ::epoll_create1(EPOLL_CLOEXEC);
		if(-1 == the_handle)
			env::Fail::fire(cs_epoll_create1_fail);

		// wake-up event
		core::assert(-1 == the_interrupt_handle);
		the_interrupt_handle = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
		if(-1 == the_interrupt_handle)
			env::Fail::fire(cs_eventfd_fail);
		struct epoll_event event = { 0 };
		event.events = EPOLLIN;
		event.data.u64 = interrupt_flag;
		if(-1 == ::epoll_ctl(the_handle, EPOLL_CTL_ADD, the_interrupt_handle, &event))
			env::Fail::fire(cs_epoll_ctl_fail);
	}
	void close()
	{
		if(-1 != the_handle)
		{
			::close(the_handle);
			the_handle = -1;
		}
		if(-1 != the_interrupt_handle)
		{
			::close(the_interrupt_handle);
			the_interrupt_handle = -1;
		}
	}
	void interrupt()
	{
		const core::uint64 cnt = 1;
		::write(the_interrupt_handle, &cnt, sizeof(cnt));
	}
	void attach(Inet& client)
	{
		struct epoll_event event = { 0 };
		event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
		event.data.u64 = (core::uint32)&client;
		if(-1 == ::epoll_ctl(the_handle, EPOLL_CTL_ADD, client.the_handle, &event))
			env::Fail::fire(cs_epoll_ctl_fail);
	}
	void attach(InetServer& server)
	{
		struct epoll_event event = { 0 };
		event.events = EPOLLIN;
		event.data.u64 = (core::uint32)&server | server_flag;
		if(-1 == ::epoll_ctl(the_handle, EPOLL_CTL_ADD, server.the_handle, &event))
			env::Fail::fire(cs_epoll_ctl_fail);
	}
	Type wait(Inet::share& inet)
	{
		if(!the_last)
		{
			the_last = ::epoll_wait(the_handle, the_events, the_max_size, -1);
			if(-1 == the_last)
				env::Fail::fire(cs_epoll_wait_fail);
		}
		if(the_last)
		{
			--the_last;
			struct epoll_event& event = the_events[the_last];
			core::uint32 flags = event.events;
			core::uint64 case_flag = event.data.u64;
			inet = *(Inet*)event.data.u64;
			if(interrupt_flag & case_flag)
				return interrupted;
			else if(server_flag & case_flag)
				return connect;
			else if((EPOLLRDHUP | EPOLLHUP) & flags)
				return disconnect;
			else if(EPOLLIN & flags)
				return read;
		}
		return interrupted;
	}
private:
	enum
	{
		interrupt_flag = 1LL << 32,
		server_flag = 2LL << 32,
		the_max_size = 0xF
	};
private:
	core::int32 the_last;
	core::int32 the_handle;
	core::int32 the_interrupt_handle;
	struct epoll_event the_events[the_max_size];
};

NAMESPACE_END(io);

#endif// IO_INET_STREAM_HPP
