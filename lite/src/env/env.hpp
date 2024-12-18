
#ifndef ENV_HPP
#define ENV_HPP

#include <time.h>
#include <unistd.h>
#include <libiberty/demangle.h>
#include <bfd.h>
#include <execinfo.h>
#include <signal.h>

namespace env
{
#include "env/runtime.hpp"
#include "env/exception.hpp"
#include "env/device.hpp"
#include "env/util.hpp"
#include "env/console.hpp"
#include "env/os.hpp"
#include "env/time.hpp"
}

#endif // ENV_HPP
