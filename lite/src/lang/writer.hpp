
class Writer
{
public:
    static void execute(Tree& tree)
    {
        Writer writer;
        writer.execute_tree(tree);
    }
private:
    Writer() : var_list(0xA)
    {
        llvm::InitializeNativeTarget();
    }
    ~Writer()
    {
        llvm::llvm_shutdown();
    }
    void execute_tree(Tree& tree)
    {
        // start main function
        the_module = llvm::make_unique<llvm::Module>("test", the_context);
        the_func = llvm::cast<llvm::Function>(the_module->getOrInsertFunction("main", llvm::Type::getInt32Ty(the_context), llvm::Type::getInt32Ty(the_context), nullptr));
        the_bb = llvm::BasicBlock::Create(the_context, "entry", the_func);

        // build body of main function
        for(auto& it : tree.var_list())
        {
            auto value = execute(it.value());
            var_list.append(value);

        }

        // return var-last * arg-0
        for(auto& arg : the_func->args())
        {
            arg.setName("arg");
            llvm::IRBuilder<> builder(the_bb);
            auto ret = builder.CreateMul(var_list[0], &arg, "return");
            builder.CreateRet(ret);
            break;
        }

        // call generated main function
        llvm::outs() << "LLVM module:\n" << *the_module.get();
        auto exec_engine = llvm::EngineBuilder(std::move(the_module)).create();
        std::vector<llvm::GenericValue> args(1);
        args[0].IntVal = llvm::APInt(32, 1);
        auto ret = exec_engine->runFunction(the_func, args);
        int result = ret.IntVal.getSExtValue();
        delete exec_engine;

        core::verify(11 == result);
    }
    llvm::AllocaInst* execute(Var& var)
    {
        auto& type = typeid(var);
        if(type == typeid(SimpleVar))
            return execute(dynamic_cast<SimpleVar&>(var));
        else if(type == typeid(ExtendedVar))
            return execute(dynamic_cast<ExtendedVar&>(var));
        else
            throw_bas_class(type);
        return 0;
    }
    llvm::AllocaInst* execute(SimpleVar& var)
    {
        auto value = execute(var.expression);
        llvm::IRBuilder<> builder(the_bb);
        auto alloca = builder.CreateAlloca(llvm::Type::getInt32Ty(the_context), nullptr, var.id.ascii());
        builder.CreateStore(value, alloca);
        return alloca;
    }
    llvm::AllocaInst* execute(ExtendedVar& var)
    {
        core::verify(false);
        return 0;
    }
    llvm::Value* execute(Expression& exp)
    {
        auto& type = typeid(exp);
        if(type == typeid(FinalExpression))
            return execute(dynamic_cast<FinalExpression&>(exp));
        else if(type == typeid(LocationExpression))
            return execute(dynamic_cast<LocationExpression&>(exp));
        else if(type == typeid(FinalNestedExpression))
            return execute(dynamic_cast<FinalNestedExpression&>(exp));
        else if(type == typeid(LocationNestedExpression))
            return execute(dynamic_cast<LocationNestedExpression&>(exp));
        else if(type == typeid(NestedExpression))
            return execute(dynamic_cast<NestedExpression&>(exp));
        else
            throw_bas_class(type);
        return 0;
    }
    llvm::Value* execute(FinalExpression& exp)
    {
        return execute(exp.final);
    }
    llvm::Value* execute(LocationExpression& exp)
    {
        return execute(exp.loc);
    }
    llvm::Value* execute(FinalNestedExpression& exp)
    {
        auto nest = execute(exp.nest);
        auto final = execute(exp.final);
        return execute(nest, exp.op, final);
    }
    llvm::Value* execute(LocationNestedExpression& exp)
    {
        auto nest = execute(exp.nest);
        auto loc = execute(exp.loc);
        return execute(nest, exp.op, loc);
    }
    llvm::Value* execute(NestedExpression& exp)
    {
        auto v1 = execute(exp.nest1);
        auto v2 = execute(exp.nest2);
        return execute(v1, exp.op, v2);
    }
    llvm::Value* execute(Location& loc)
    {
        auto& type = typeid(loc);
        if(type == typeid(IdLocation))
            return execute(dynamic_cast<IdLocation&>(loc));
        else if(type == typeid(SeqLocation))
            return execute(dynamic_cast<SeqLocation&>(loc));
        else if(type == typeid(NestedLocation))
            return execute(dynamic_cast<NestedLocation&>(loc));
        else if(type == typeid(NestedSeqLocation))
            return execute(dynamic_cast<NestedSeqLocation&>(loc));
        else
            throw_bas_class(type);
        return 0;
    }
    llvm::Value* execute(IdLocation& loc)
    {
        for(auto it : var_list)
        {
            auto value = it.value();
            if(loc.id.equal(value->getName().data()))
                return value;
        }
        env::Throw("Unknown variable: $1").arg(loc.id).end();
        return 0;
    }
    llvm::Value* execute(SeqLocation& loc)
    {
        core::verify(false);
        return 0;
    }
    llvm::Value* execute(NestedLocation& loc)
    {
        core::verify(false);
        return 0;
    }
    llvm::Value* execute(NestedSeqLocation& loc)
    {
        core::verify(false);
        return 0;
    }
    llvm::Value* execute(Final& final)
    {
        auto i = final.value.to_int32();
        llvm::IRBuilder<> builder(the_bb);
        return builder.getInt32(i);
    }
    llvm::Value* execute(llvm::Value* v1, Operator op, llvm::Value* v2)
    {
        llvm::IRBuilder<> builder(the_bb);
        switch(op)
        {
            case Operator::MUL:
                return builder.CreateMul(v1, v2);
            case Operator::DIV:
                return builder.CreateUDiv(v1, v2);
            case Operator::ADD:
                return builder.CreateAdd(v1, v2);
            case Operator::SUB:
                return builder.CreateSub(v1, v2);
            case Operator::SHL:
                return builder.CreateShl(v1, v2);
            case Operator::SHR:
                return builder.CreateLShr(v1, v2);
            case Operator::EQ:
            case Operator::NE:
            case Operator::LT:
            case Operator::GT:
            case Operator::LE:
            case Operator::GE:
            case Operator::AND:
                return builder.CreateAnd(v1, v2);
            case Operator::OR:
                return builder.CreateOr(v1, v2);
            case Operator::XOR:
                return builder.CreateXor(v1, v2);
            case Operator::MOD:
            case Operator::NOT:
            default:
                env::Throw::raise("Unknown binary operator");
        }
        return 0;
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
    core::List<llvm::AllocaInst*> var_list;
};
