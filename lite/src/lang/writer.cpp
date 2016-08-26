
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
  
  std::unique_ptr<Module> Owner = make_unique<Module>("test", Context);
  Module *M = Owner.get();

  Function *Add1F =
    cast<Function>(M->getOrInsertFunction("add1", Type::getInt32Ty(Context),
                                          Type::getInt32Ty(Context),
                                          nullptr));

  BasicBlock *BB = BasicBlock::Create(Context, "entry", Add1F);
  IRBuilder<> builder(BB);

  Value *One = builder.getInt32(1);
  assert(Add1F->arg_begin() != Add1F->arg_end()); // Make sure there's an arg
  Argument *ArgX = &*Add1F->arg_begin();          // Get the arg
  ArgX->setName("an_arg");            // Give it a nice symbolic name for fun.

  Value *Add = builder.CreateAdd(One, ArgX);

  builder.CreateRet(Add);
  Function *FooF =
    cast<Function>(M->getOrInsertFunction("foo", Type::getInt32Ty(Context),
                                          nullptr));
  BB = BasicBlock::Create(Context, "entry", FooF);

  builder.SetInsertPoint(BB);

  Value *Ten = builder.getInt32(10);
  CallInst *Add1CallRes = builder.CreateCall(Add1F, Ten);
  Add1CallRes->setTailCall(true);

  builder.CreateRet(Add1CallRes);

  ExecutionEngine* EE = EngineBuilder(std::move(Owner)).create();

  outs() << "We just constructed this LLVM module:\n\n" << *M;
  outs() << "\n\nRunning foo: ";
  outs().flush();

  std::vector<GenericValue> noargs;
  GenericValue gv = EE->runFunction(FooF, noargs);

  outs() << "Result: " << gv.IntVal << "\n";
  delete EE;
  llvm_shutdown();
}
}
