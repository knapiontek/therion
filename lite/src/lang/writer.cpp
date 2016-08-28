
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

using namespace llvm;

namespace lang
{
void writer_write()
{
  
  InitializeNativeTarget();

  LLVMContext Context;
  
  auto module = make_unique<Module>("test", Context);

  auto add1_func = cast<Function>(module->getOrInsertFunction("add1",
							      Type::getInt32Ty(Context),
							      Type::getInt32Ty(Context),
							      nullptr));

  auto bb = BasicBlock::Create(Context, "entry", add1_func);
  IRBuilder<> builder(bb);

  Value *one_val = builder.getInt32(1);
  assert(add1_func->arg_begin() != add1_func->arg_end());
  auto x_arg = &*add1_func->arg_begin();
  x_arg->setName("an_arg");

  auto add_val = builder.CreateAdd(one_val, x_arg);

  builder.CreateRet(add_val);
  auto foo_func = cast<Function>(module->getOrInsertFunction("foo", Type::getInt32Ty(Context), nullptr));
  bb = BasicBlock::Create(Context, "entry", foo_func);

  builder.SetInsertPoint(bb);

  auto ten_val = builder.getInt32(10);
  auto add1_res = builder.CreateCall(add1_func, ten_val);
  add1_res->setTailCall(true);

  builder.CreateRet(add1_res);

  outs() << "We just constructed this LLVM module:\n\n" << *module.get();

  auto exec_engine = EngineBuilder(std::move(module)).create();

  outs() << "\n\nRunning foo: ";
  outs().flush();

  std::vector<GenericValue> noargs;
  auto val = exec_engine->runFunction(foo_func, noargs);
  outs() << "Result: " << val.IntVal << "\n";

  delete exec_engine;
  llvm_shutdown();
}
}
