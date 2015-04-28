
#ifndef LAYGEN_CS_HPP
#define LAYGEN_CS_HPP

#include "core/core/string.hpp"

core::String cs_verbose_switch = cs("-v");
core::String cs_input_switch = cs("-i");
core::String cs_output_switch = cs("-o");
core::String cs_usage = cs("usage: laygen [-v] -i file.lay -o file.hpp");
core::String cs_unknown_exception = cs("unknown exception");
core::String cs_max_tabs_allowed = cs("$1:$2 max tabs allowed: $3");
core::String cs_syntax_error = cs("$1:$2 syntax error");
core::String cs_trailing_tabs = cs("$1:$2 trailing tabs");
core::String cs_unknown_type = cs("$1:$2 unknown type: $3");

#endif // LAYGEN_CS_HPP
