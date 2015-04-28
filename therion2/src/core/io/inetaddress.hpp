
#ifndef IO_INET_ADDRESS_HPP
#define IO_INET_ADDRESS_HPP

#include "core/io/input.hpp"
#include "core/io/output.hpp"
#include "core/core/seq.hpp"
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <errno.h>

const in_addr inaddr_any = { 0 };
const in_addr inaddr_loopback = { 16777343 };

NAMESPACE_BEGIN(io);

class InetAddress
{
	friend class InetLocal;
	friend class InetClient;
	friend class InetServer;
public:
	typedef core::Shared<InetAddress> share;
public:
	static bool lookup(core::Seq<InetAddress>& address_seq, core::String& host, core::String& service)
	{
		InetAddress address;
		struct addrinfo hints;
		::bzero(&hints, sizeof(hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		struct addrinfo* head;
		if(!::getaddrinfo(host.ascii(), service.ascii(), &hints, &head))
		{
			struct addrinfo* it = head;
			while(it)
			{
				switch(it->ai_family)
				{
				case AF_INET6:
					address.the_sockaddr_in6 = *(sockaddr_in6*)it->ai_addr;
					address_seq.append(address);
					break;
				case AF_INET:
					address.the_sockaddr_in = *(sockaddr_in*)it->ai_addr;
					address_seq.append(address);
					break;
				}
				it = it->ai_next;
			}
			::freeaddrinfo(head);
			return true;
		}
		return false;
	}
public:
	InetAddress()
	{
		::bzero(&the_sockaddr, sizeof(sockaddr_storage));
	}
	bool to_hostname(core::String& host, core::uint16& port)
	{
		char host_buffer[NI_MAXHOST];
		char port_buffer[NI_MAXSERV];
		bool result = !::getnameinfo(&the_sockaddr, sizeof(sockaddr_storage),
			host_buffer, NI_MAXHOST,
			port_buffer, NI_MAXSERV,
			NI_NUMERICSERV);
		if(result)
		{
			host = ascii_nlen(host_buffer);
			port = ascii_nlen(port_buffer).to_uint16();
			result = (core::uint16_nil != port);
		}
		return result;
	}
	void any_ipv6(core::uint16 port = 0)
	{
		the_sockaddr_in6.sin6_family = AF_INET6;
		the_sockaddr_in6.sin6_port = htons(port);
		the_sockaddr_in6.sin6_addr = in6addr_any;
	}
	void any_ipv4(core::uint16 port = 0)
	{
		the_sockaddr_in.sin_family = AF_INET;
		the_sockaddr_in.sin_port = htons(port);
		the_sockaddr_in.sin_addr = inaddr_any;
	}
	void loopback_ipv6(core::uint16 port = 0)
	{
		the_sockaddr_in6.sin6_family = AF_INET6;
		the_sockaddr_in6.sin6_port = htons(port);
		the_sockaddr_in6.sin6_addr = in6addr_loopback;
	}
	void loopback_ipv4(core::uint16 port = 0)
	{
		the_sockaddr_in.sin_family = AF_INET;
		the_sockaddr_in.sin_port = htons(port);
		the_sockaddr_in.sin_addr = inaddr_loopback;
	}
	bool from_ip_format(core::String& host, core::uint16 port = 0)
	{
		if(1 == ::inet_pton(AF_INET6, host.ascii(), &the_sockaddr_in6.sin6_addr))
		{
			the_sockaddr_in6.sin6_family = AF_INET6;
			the_sockaddr_in6.sin6_port = htons(port);
			return true;
		}
		else if(1 == ::inet_pton(AF_INET, host.ascii(), &the_sockaddr_in.sin_addr))
		{
			the_sockaddr_in.sin_family = AF_INET;
			the_sockaddr_in.sin_port = htons(port);
			return true;
		}
		return false;
	}
	bool to_ip_format(core::String& host, core::uint16& port)
	{
		const core::uint32 size = 32;
		char buffer[size];
		switch(the_sockaddr.sa_family)
		{
		case AF_INET6:
			::inet_ntop(the_sockaddr_in6.sin6_family, &the_sockaddr_in6.sin6_addr, buffer, size);
			host = ascii_nlen(buffer);
			port = ntohs(the_sockaddr_in6.sin6_port);
			return true;
		case AF_INET:
			::inet_ntop(the_sockaddr_in.sin_family, &the_sockaddr_in.sin_addr, buffer, size);
			host = ascii_nlen(buffer);
			port = ntohs(the_sockaddr_in.sin_port);
			return true;
		}
		return false;
	}
private:
	union
	{
		struct sockaddr the_sockaddr;
		sockaddr_in the_sockaddr_in;
		sockaddr_in6 the_sockaddr_in6;
		sockaddr_storage the_storage;
	};
};

class InetLocal
{
public:
	enum
	{
		run = IFF_RUNNING,
		lo = IFF_LOOPBACK
	};
public:
	static bool inet_info(core::Seq<InetLocal>& local_seq)
	{
		InetLocal local;
		struct ifaddrs* head;
		if(!::getifaddrs(&head))
		{
			struct ifaddrs* it = head;
			while(it)
			{
				switch(it->ifa_addr->sa_family)
				{
				case AF_INET6:
					local.the_address.the_sockaddr_in6 = *(sockaddr_in6*)it->ifa_addr;
					local.the_status = it->ifa_flags;
					local_seq.append(local);
					break;
				case AF_INET:
					local.the_address.the_sockaddr_in = *(sockaddr_in*)it->ifa_addr;
					local.the_status = it->ifa_flags;
					local_seq.append(local);
					break;
				}
				it = it->ifa_next;
			}
			::freeifaddrs(head);
			return true;
		}
		return false;
	}
	static void any(InetAddress& address, core::uint16 port = 0)
	{
		can_any_ipv6()
			? address.any_ipv6(port)
			: address.any_ipv4(port);
	}
	static void loopback(InetAddress& address, core::uint16 port = 0)
	{
		can_loopback_ipv6()
			? address.loopback_ipv6(port)
			: address.loopback_ipv4(port);
	}
	static bool can_any_ipv6()
	{
		struct ifaddrs* head;
		if(!::getifaddrs(&head))
		{
			struct ifaddrs* it = head;
			while(it)
			{
				if(AF_INET6 == it->ifa_addr->sa_family
					&& IFF_RUNNING & it->ifa_flags
					&& !(IFF_LOOPBACK & it->ifa_flags))
				{
					::freeifaddrs(head);
					return true;
				}
				it = it->ifa_next;
			}
			::freeifaddrs(head);
		}
		return false;
	}
	static bool can_any_ipv4()
	{
		struct ifaddrs* head;
		if(!::getifaddrs(&head))
		{
			struct ifaddrs* it = head;
			while(it)
			{
				if(AF_INET == it->ifa_addr->sa_family
					&& IFF_RUNNING & it->ifa_flags
					&& !(IFF_LOOPBACK & it->ifa_flags))
				{
					::freeifaddrs(head);
					return true;
				}
				it = it->ifa_next;
			}
			::freeifaddrs(head);
		}
		return false;
	}
	static bool can_loopback_ipv6()
	{
		struct ifaddrs* head;
		if(!::getifaddrs(&head))
		{
			struct ifaddrs* it = head;
			while(it)
			{
				if(AF_INET6 == it->ifa_addr->sa_family
					&& IFF_RUNNING & it->ifa_flags
					&& IFF_LOOPBACK & it->ifa_flags)
				{
					::freeifaddrs(head);
					return true;
				}
				it = it->ifa_next;
			}
			::freeifaddrs(head);
		}
		return false;
	}
	static bool can_loopback_ipv4()
	{
		struct ifaddrs* head;
		if(!::getifaddrs(&head))
		{
			struct ifaddrs* it = head;
			while(it)
			{
				if(AF_INET == it->ifa_addr->sa_family
					&& IFF_RUNNING & it->ifa_flags
					&& IFF_LOOPBACK & it->ifa_flags)
				{
					::freeifaddrs(head);
					return true;
				}
				it = it->ifa_next;
			}
			::freeifaddrs(head);
		}
		return false;
	}
public:
	bool status(core::uint32 status)
	{
		return the_status & status;
	}
	InetAddress& address()
	{
		return the_address;
	}
private:
	core::uint32 the_status;
	InetAddress the_address;
};

NAMESPACE_END(io);

#endif // IO_INET_ADDRESS_HPP
