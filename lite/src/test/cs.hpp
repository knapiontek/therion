
#ifndef TEST_CS_HPP
#define TEST_CS_HPP

#include "core/string.hpp"

// core

core::String cs_first = cs("first");
core::String cs_third = cs("third");
core::String cs_flower = cs("flower");
core::String cs_poppy = cs("poppy");
core::String cs_notnil = cs("notnil");
core::String cs_punctuation = cs("!\"£$^&*()_+{}[]:@~;'#|\\<>?,./");
core::String cs_string = cs(" string: $ $");
core::String cs_data = cs(" data: ");
core::String cs_bool = cs(" bool: ");
core::String cs_int8 = cs(" int8: ");
core::String cs_int16 = cs(" int16: ");
core::String cs_int32 = cs(" int32: ");
core::String cs_int64 = cs(" int64: ");
core::String cs_uint8 = cs(" uint8: ");
core::String cs_uint16 = cs(" uint16: ");
core::String cs_uint32 = cs(" uint32: ");
core::String cs_uint64 = cs(" uint64: ");
core::String cs_float32 = cs(" float32: ");
core::String cs_float64 = cs(" float64: ");
core::String cs_float128 = cs(" float128: ");
core::String cs_and_the_end = cs(" and the end$");
core::String cs_mix = cs("-127false-9223372036854775807");
core::String cs_format = cs(" string: $$ $$$1 data: $2 bool: $3 int8: $4 int16: $A int32: $6 int64: $7 uint8: $8 uint16: $9 uint32: $5 uint64: $B float32: $C float64: $D float128: $E and the end$$");
core::String cs_hardcoded_output = cs(" string: $ $!\"£$^&*()_+{}[]:@~;'#|\\<>?,./ data: *** bool: false int8: -127 int16: -32767 int32: -2147483647 int64: -9223372036854775807 uint8: 127 uint16: 32767 uint32: 2147483647 uint64: 9223372036854775807 float32: 123456.78 float64: 1234567890.123456 float128: 1234567890.1234567 and the end$");

// thread

core::String cs_synch_thread = cs("synch-thread");
core::String cs_client_thread = cs("client-thread");
core::String cs_server_thread = cs("server-thread");
core::String cs_listen_thread = cs("listen-thread");
core::String cs_producer_thread = cs("producer-thread");

// env

core::String cs_message = cs("exception test 10.00000");
core::String cs_exception_test = cs("exception test $1");
core::String cs_name = cs("name");
core::String cs_value = cs("value");
core::String cs_sh = cs("sh");

// io

core::String cs_filetest_tmp = cs("/tmp/filetest.tmp");
core::String cs_tmp = cs(".tmp");
core::String cs_file_format = cs("$1/$2.$3");
core::String cs_slash = cs("/");
core::String cs_host = cs("::1");
core::String cs_outgoing = cs("the_outgoing");
core::String cs_incoming = cs("the_incoming");
core::String cs_www_google_com = cs("www.google.com");
core::String cs_http = cs("http");

#endif // TEST_CS_HPP

