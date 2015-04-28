
#ifndef SERVICE_CS_HPP
#define SERVICE_CS_HPP

#include "core/core/string.hpp"

core::String cs_unknown_exception = cs("unknown exception");

core::String cs_address_format = cs(" [$1]:$2");

core::String cs_bind_fail =
	cs_eng("bind address fail:")
	cs_pl("błąd (bind) adres:");

#endif // SERVICE_CS_HPP
