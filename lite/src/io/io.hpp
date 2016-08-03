
#ifndef IO_HPP
#define IO_HPP

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

namespace io
{
#include "io/input.hpp"
#include "io/output.hpp"
#include "io/file.hpp"
}

#endif // IO_HPP
