
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

namespace lang
{
void writer_write()
{
    // initialize
    llvm::InitializeNativeTarget();
    llvm::LLVMContext context;
    auto module = llvm::make_unique<llvm::Module>("test", context);

    // add1
    auto add1_func = llvm::cast<llvm::Function>(
        module->getOrInsertFunction("add1",
            llvm::Type::getInt32Ty(context),
            llvm::Type::getInt32Ty(context),
            nullptr)
    );
    auto bb = llvm::BasicBlock::Create(context, "entry", add1_func);
    llvm::IRBuilder<> builder(bb);

    auto one_val = builder.getInt32(1);
    auto x_arg = &*add1_func->arg_begin();
    x_arg->setName("an_arg");
    auto add_val = builder.CreateAdd(one_val, x_arg);
    builder.CreateRet(add_val);

    // foo
    auto foo_func = llvm::cast<llvm::Function>(module->getOrInsertFunction("foo", llvm::Type::getInt32Ty(context), nullptr));
    bb = llvm::BasicBlock::Create(context, "entry", foo_func);
    builder.SetInsertPoint(bb);

    auto ten_val = builder.getInt32(10);
    auto add1_res = builder.CreateCall(add1_func, ten_val);
    add1_res->setTailCall(true);
    builder.CreateRet(add1_res);

    // execution engine
    llvm::outs() << "We just constructed this LLVM module:\n\n" << *module.get();
    auto exec_engine = llvm::EngineBuilder(std::move(module)).create();
    llvm::outs() << "\n\nRunning foo: ";
    llvm::outs().flush();
    std::vector<llvm::GenericValue> noargs;
    auto val = exec_engine->runFunction(foo_func, noargs);
    llvm::outs() << "Result: " << val.IntVal << "\n";

    // close
    delete exec_engine;
    llvm::llvm_shutdown();
}
}
