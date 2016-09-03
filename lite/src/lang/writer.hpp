
class Writer
{
public:
    static void execute(Tree& tree)
    {
        Writer writer;
        for(auto& it : tree.var_list())
        {
            writer.execute(it.value());
        }
        writer.run_llvm();
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
            throw_bas_class(type);
        return value;
    }
    llvm::Value* execute(SimpleVar& var)
    {
        auto value = execute(var.expression);
        llvm::IRBuilder<> builder(the_bb);
        builder.CreateRet(value);
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
            throw_bas_class(type);
        return value;
    }
    llvm::Value* execute(FinalExpression& exp)
    {
        return execute(exp.final);
    }
    llvm::Value* execute(LocationExpression& exp)
    {
        llvm::Value* value = 0;
        return value;
    }
    llvm::Value* execute(FinalNestedExpression& exp)
    {
        auto nest = execute(exp.nest);
        auto final = execute(exp.final);
        llvm::IRBuilder<> builder(the_bb);
        switch(exp.op)
        {
            case Operator::MUL:
                return builder.CreateMul(nest, final);
            case Operator::DIV:
                return builder.CreateUDiv(nest, final);
            case Operator::ADD:
                return builder.CreateAdd(nest, final);
            case Operator::SUB:
                return builder.CreateSub(nest, final);
            case Operator::SHL:
                return builder.CreateShl(nest, final);
            case Operator::SHR:
                return builder.CreateLShr(nest, final);
            case Operator::EQ:
            case Operator::NE:
            case Operator::LT:
            case Operator::GT:
            case Operator::LE:
            case Operator::GE:
            case Operator::AND:
                return builder.CreateAnd(nest, final);
            case Operator::OR:
                return builder.CreateOr(nest, final);
            case Operator::XOR:
                return builder.CreateXor(nest, final);
            case Operator::MOD:
            case Operator::NOT:
            default:
                env::Throw::raise("Unknown binary operator");
        }
        return 0;
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
            throw_bas_class(type);
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
    llvm::Value* execute(Final& final)
    {
        auto i = final.value.to_int32();
        llvm::IRBuilder<> builder(the_bb);
        return builder.getInt32(i);
    }
    void open_llvm()
    {
        llvm::InitializeNativeTarget();
        the_module = llvm::make_unique<llvm::Module>("test", the_context);
        the_func = llvm::cast<llvm::Function>(the_module->getOrInsertFunction("main", llvm::Type::getInt32Ty(the_context), llvm::Type::getInt32Ty(the_context), nullptr));
        the_bb = llvm::BasicBlock::Create(the_context, "entry", the_func);
    }
    void close_llvm()
    {
        llvm::llvm_shutdown();
    }
    void run_llvm()
    {
        llvm::outs() << "LLVM module:\n" << *the_module.get();
        auto result = run_function(the_func);
        core::verify(11 == result);
    }
    int run_function(llvm::Function* func)
    {
        auto exec_engine = llvm::EngineBuilder(std::move(the_module)).create();
        std::vector<llvm::GenericValue> args(1);
        args[0].IntVal = llvm::APInt(32, 13);
        auto val = exec_engine->runFunction(func, args);
        int result = val.IntVal.getSExtValue();
        delete exec_engine;
        return result;
    }
    void throw_bas_class(const std::type_info& info)
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
