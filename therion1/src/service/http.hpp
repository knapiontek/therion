
#ifndef SERVICE_HTTP_HPP
#define SERVICE_HTTP_HPP

#include "core/io/inetstream.hpp"

NAMESPACE_BEGIN(service);

class Transport
{
public:
	typedef core::Shared<Transport> share;
public:
	virtual ~Transport()
	{

	}
	virtual core::String read_target(io::Inet& inet)
	{
		return core::String::nil();
	}
};

class TherionTransport : public Transport
{
public:
	typedef core::Shared<TherionTransport> share;
public:
	virtual ~TherionTransport()
	{

	}
	virtual core::String read_target(io::Inet& inet)
	{
		return core::String::nil();
	}
};

class HttpTransport : public Transport
{
public:
	typedef core::Shared<HttpTransport> share;
public:
	virtual ~HttpTransport()
	{

	}
	virtual core::String read_target(io::Inet& inet)
	{
		return core::String::nil();
	}
};

NAMESPACE_END(service);

#endif // SERVICE_HTTP_HPP

