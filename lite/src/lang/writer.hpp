
class Writer
{
public:
    static void execute(Tree& tree)
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
        //llvm::outs() << "We just constructed this LLVM module:\n\n" << *module.get();
        auto exec_engine = llvm::EngineBuilder(std::move(module)).create();
        std::vector<llvm::GenericValue> noargs;
        auto val = exec_engine->runFunction(foo_func, noargs);
        int result = val.IntVal.getSExtValue();
        core::verify(11 == result);

        // close
        delete exec_engine;
        llvm::llvm_shutdown();
    }
    void execute(Loc& loc)
    {
        (void)loc;
    }
    void execute(Exp& exp)
    {
        (void)exp;
    }
    void execute(BinaryOpExp& binary_op_exp)
    {
        (void)binary_op_exp;
    }
    void execute(Var& var)
    {
        (void)var;
    }
};
