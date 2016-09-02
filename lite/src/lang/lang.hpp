
#ifndef LANG_HPP
#define LANG_HPP

#include <typeinfo>

/*
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS
#define __STDC_LIMIT_MACROS

#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
*/
namespace lang
{
#include "lang/tree.hpp"
#include "lang/grammar.hpp"
#include "lang/grammar.cpp"
#include "lang/reader.hpp"
#include "lang/writer.hpp"
}

#endif // LANG_HPP
