
class Writer
{
public:
    static void execute(Tree& tree)
    {
        Writer writer;
        writer.llvm_test();
        for(auto& it : tree.var_list())
        {
            writer.execute(it.value());
        }
    }
private:
    llvm::Value* execute(Var& var)
    {
        llvm::Value* value = 0;
        auto& type = typeid(var);
        if(type == typeid(SimpleVar))
            value = execute(dynamic_cast<SimpleVar&>(var));
        else if(type == typeid(ExtendedVar))
            value = execute(dynamic_cast<ExtendedVar&>(var));
        else
            throw_exception(type);
        return value;
    }
    llvm::Value* execute(SimpleVar& var)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(ExtendedVar& var)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(Expression& exp)
    {
        llvm::Value* value = 0;
        auto& type = typeid(exp);
        if(type == typeid(FinalExpression))
            value = execute(dynamic_cast<FinalExpression&>(exp));
        else if(type == typeid(LocationExpression))
            value = execute(dynamic_cast<LocationExpression&>(exp));
        else if(type == typeid(FinalNestedExpression))
            value = execute(dynamic_cast<FinalNestedExpression&>(exp));
        else if(type == typeid(LocationNestedExpression))
            value = execute(dynamic_cast<LocationNestedExpression&>(exp));
        else if(type == typeid(NestedExpression))
            value = execute(dynamic_cast<NestedExpression&>(exp));
        else
            throw_exception(type);
        return value;
    }
    llvm::Value* execute(FinalExpression& exp)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(LocationExpression& exp)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(FinalNestedExpression& exp)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(LocationNestedExpression& exp)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(NestedExpression& exp)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(Location& loc)
    {
        llvm::Value* value = 0;
        auto& type = typeid(loc);
        if(type == typeid(IdLocation))
            value = execute(dynamic_cast<IdLocation&>(loc));
        else if(type == typeid(SeqLocation))
            value = execute(dynamic_cast<SeqLocation&>(loc));
        else if(type == typeid(NestedLocation))
            value = execute(dynamic_cast<NestedLocation&>(loc));
        else if(type == typeid(NestedSeqLocation))
            value = execute(dynamic_cast<NestedSeqLocation&>(loc));
        else
            throw_exception(type);
        return value;
    }
    llvm::Value* execute(IdLocation& loc)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(SeqLocation& loc)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(NestedLocation& loc)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(NestedSeqLocation& loc)
    {
        llvm::Value* value = 0;
        return value;
    }
    void llvm_test()
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
    void throw_exception(const std::type_info& info)
    {
        env::Throw("Writable: $1 not handled")
            .arg(info.name())
            .end();
    }
};
