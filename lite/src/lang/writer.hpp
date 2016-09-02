
class Writer
{
public:
    static void execute(Tree& tree)
    {
        /*
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
        */

        Writer writer;
        for(auto& it : tree.var_list())
        {
            writer.execute(it.value());
        }
    }
    typedef int Return;
    Return zero;
    Return& execute(Writable& writable)
    {
        auto& type = typeid(writable);
        if(type == typeid(IdLocation))
            return execute(dynamic_cast<IdLocation&>(writable));
        else if(type == typeid(SeqLocation))
            return execute(dynamic_cast<SeqLocation&>(writable));
        else if(type == typeid(NestedLocation))
            return execute(dynamic_cast<NestedLocation&>(writable));
        else if(type == typeid(NestedSeqLocation))
            return execute(dynamic_cast<NestedSeqLocation&>(writable));
        else if(type == typeid(TypeExpression))
            return execute(dynamic_cast<TypeExpression&>(writable));
        else if(type == typeid(LocationExpression))
            return execute(dynamic_cast<LocationExpression&>(writable));
        else if(type == typeid(TypeNestedExpression))
            return execute(dynamic_cast<TypeNestedExpression&>(writable));
        else if(type == typeid(LocationNestedExpression))
            return execute(dynamic_cast<LocationNestedExpression&>(writable));
        else if(type == typeid(NestedExpression))
            return execute(dynamic_cast<NestedExpression&>(writable));
        else if(type == typeid(VarImpl))
            return execute(dynamic_cast<VarImpl&>(writable));
        else
            env::Throw("Writable: $1 not handled").arg(typeid(writable).name()).end();
        return zero;
    }
    Return& execute(IdLocation& loc)
    {
        return zero;
    }
    Return& execute(SeqLocation& loc)
    {
        return zero;
    }
    Return& execute(NestedLocation& loc)
    {
        return zero;
    }
    Return& execute(NestedSeqLocation& loc)
    {
        return zero;
    }
    Return& execute(TypeExpression& exp)
    {
        return zero;
    }
    Return& execute(LocationExpression& exp)
    {
        return zero;
    }
    Return& execute(TypeNestedExpression& exp)
    {
        return zero;
    }
    Return& execute(LocationNestedExpression& exp)
    {
        return zero;
    }
    Return& execute(NestedExpression& exp)
    {
        return zero;
    }
    Return& execute(VarImpl& var)
    {
        return zero;
    }
};
