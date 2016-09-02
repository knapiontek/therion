
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
    Writer()
    {
        open_llvm();
    }
    ~Writer()
    {
        close_llvm();
    }
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
        execute(var.expression);
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
        auto f = exp.final->value.to_float64();
        (void)f;
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
    void open_llvm()
    {
        llvm::InitializeNativeTarget();
        the_module = llvm::make_unique<llvm::Module>("test", the_context);
        the_func = llvm::cast<llvm::Function>(the_module->getOrInsertFunction("main", llvm::Type::getInt32Ty(the_context), nullptr));
        the_bb = llvm::BasicBlock::Create(the_context, "entry", the_func);
    }
    void close_llvm()
    {
        llvm::llvm_shutdown();
    }
    void llvm_test()
    {
        // add1
        auto add1_func = llvm::cast<llvm::Function>(
            the_module->getOrInsertFunction("add1",
                llvm::Type::getInt32Ty(the_context),
                llvm::Type::getInt32Ty(the_context),
                nullptr)
        );
        auto bb = llvm::BasicBlock::Create(the_context, "entry", add1_func);
        llvm::IRBuilder<> builder(bb);

        auto one_val = builder.getInt32(1);
        auto x_arg = &*add1_func->arg_begin();
        x_arg->setName("an_arg");
        auto add_val = builder.CreateAdd(one_val, x_arg);
        builder.CreateRet(add_val);

        // foo
        auto foo_func = llvm::cast<llvm::Function>(the_module->getOrInsertFunction("foo", llvm::Type::getInt32Ty(the_context), nullptr));
        bb = llvm::BasicBlock::Create(the_context, "entry", foo_func);
        builder.SetInsertPoint(bb);

        auto ten_val = builder.getInt32(10);
        auto add1_res = builder.CreateCall(add1_func, ten_val);
        add1_res->setTailCall(true);
        builder.CreateRet(add1_res);

        llvm::outs() << "We just constructed this LLVM module:\n\n" << *the_module.get();
        auto result = run_function(foo_func);
        core::verify(11 == result);
    }
    int run_function(llvm::Function* func)
    {
        auto exec_engine = llvm::EngineBuilder(std::move(the_module)).create();
        std::vector<llvm::GenericValue> noargs;
        auto val = exec_engine->runFunction(func, noargs);
        int result = val.IntVal.getSExtValue();
        delete exec_engine;
        return result;
    }
    void throw_exception(const std::type_info& info)
    {
        env::Throw("Writable: $1 not handled")
            .arg(info.name())
            .end();
    }
private:
    llvm::LLVMContext the_context;
    std::unique_ptr<llvm::Module> the_module;
    llvm::Function* the_func;
    llvm::BasicBlock* the_bb;
};
