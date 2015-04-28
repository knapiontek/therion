
#ifndef OBGEN_CS_HPP
#define OBGEN_CS_HPP

#include "core/core/string.hpp"

core::String cs_syntax_error = cs("$1:$2 syntax error");
core::String cs_unknown_id = cs("$1:$2 unknown: $3");
core::String cs_unknown_index = cs("$1:$2 unknown: $3$4");
core::String cs_redefined_id = cs("$1:$2 redefined: $3");
core::String cs_verbose_switch = cs("-v");
core::String cs_input_switch = cs("-i");
core::String cs_output_switch = cs("-o");
core::String cs_usage = cs("usage: obgen [-v] -i file.pro -o file.hpp");
core::String cs_unknown_exception = cs("unknown exception");

core::String cs_string_type = cs("String");

#endif // OBGEN_CS_HPP
