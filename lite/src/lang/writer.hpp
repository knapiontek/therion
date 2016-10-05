
class VirtualMachine
{
public:
    VirtualMachine()
    {
        llvm::InitializeNativeTarget();
        the_module = llvm::make_unique<llvm::Module>("test", the_context);

        //the_main = function("main", llvm::Type::INT32, { llvm::Type::VOID });
        the_entry = block("entry", the_main);
    }
    ~VirtualMachine()
    {
        llvm::llvm_shutdown();
    }
    llvm::Function* function(core::String name, llvm::Type* result, llvm::ArrayRef<llvm::Type*> args)
    {
        llvm::FunctionType* type = llvm::FunctionType::get(result, args, false);
        auto function = the_module->getFunction(name.ascii());
        if(!function)
        {
            function = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, name.ascii(), the_module.get());
        }
        return function;
    }
    llvm::BasicBlock* block(core::String name, llvm::Function* function)
    {
        return llvm::BasicBlock::Create(the_context, name.ascii(), function);
    }
private:
    llvm::LLVMContext the_context;
    std::unique_ptr<llvm::Module> the_module;
    llvm::Function* the_main;
    llvm::BasicBlock* the_entry;
};

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
        auto func = the_module->getOrInsertFunction("main",
            llvm::Type::getInt64Ty(the_context), llvm::Type::getInt32Ty(the_context),
            nullptr);
        the_func = llvm::cast<llvm::Function>(func);
        the_bb = llvm::BasicBlock::Create(the_context, "entry", the_func);

        // build body of main function
        for(auto& it : tree.var_list())
        {
            auto value = execute(it.value());
            var_list.append(value);
        }

        llvm::IRBuilder<> builder(the_bb);

        // use function argument
        for(auto& arg : the_func->args())
        {
            arg.setName("arg");
            auto var = var_list[0];
            auto var_name = var->getName().data();
            auto var_load = builder.CreateLoad(var, var_name);
            auto value = execute(var_load, Operator::MUL, &arg);
            auto alloca = builder.CreateAlloca(value->getType(), nullptr, "mul_arg");
            builder.CreateStore(value, alloca);
            break;
        }

        // create function return
        auto var = var_list[var_list.size() - 1];
        auto var_name = var->getName().data();
        auto var_load = builder.CreateLoad(var, var_name);
        builder.CreateRet(var_load);

        // call generated main function
        llvm::outs() << "LLVM module:\n" << *the_module.get();
        llvm::verifyModule(*the_module.get(), &llvm::outs());
        auto exec_engine = llvm::EngineBuilder(std::move(the_module)).create();
        std::vector<llvm::GenericValue> args(1);
        args[0].IntVal = llvm::APInt(32, 1);
        auto ret = exec_engine->runFunction(the_func, args);
        int result = ret.IntVal.getSExtValue();
        delete exec_engine;

        core::verify(!result);
    }
    llvm::AllocaInst* execute(Var& var)
    {
        if(core::type_of<SimpleVar>(var))
            return execute(core::down_cast<SimpleVar>(var));
        else if(core::type_of<ExtendedVar>(var))
            return execute(core::down_cast<ExtendedVar>(var));
        else
            throw_bad_class(var);
        return 0;
    }
    llvm::AllocaInst* execute(SimpleVar& var)
    {
        llvm::IRBuilder<> builder(the_bb);
        auto value = execute(var.expression);
        auto alloca = builder.CreateAlloca(value->getType(), nullptr, var.id.ascii());
        builder.CreateStore(value, alloca);
        return alloca;
    }
    llvm::AllocaInst* execute(ExtendedVar& var)
    {
        core::verify(var.var.type_of<SimpleVar>());
        auto& simple = var.var.down_cast<SimpleVar>();
        auto struct_type = llvm::StructType::create(the_context, simple.id.ascii());
        (void)struct_type;
        return execute(var.var);
    }
    llvm::Value* execute(Expression& exp)
    {
        if(core::type_of<FinalExpression>(exp))
            return execute(core::down_cast<FinalExpression>(exp));
        else if(core::type_of<LocationExpression>(exp))
            return execute(core::down_cast<LocationExpression>(exp));
        else if(core::type_of<FinalNestedExpression>(exp))
            return execute(core::down_cast<FinalNestedExpression>(exp));
        else if(core::type_of<LocationNestedExpression>(exp))
            return execute(core::down_cast<LocationNestedExpression>(exp));
        else if(core::type_of<NestedExpression>(exp))
            return execute(core::down_cast<NestedExpression>(exp));
        else
            throw_bad_class(exp);
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
        if(core::type_of<IdLocation>(loc))
            return execute(core::down_cast<IdLocation>(loc));
        else if(core::type_of<SeqLocation>(loc))
            return execute(core::down_cast<SeqLocation>(loc));
        else if(core::type_of<NestedLocation>(loc))
            return execute(core::down_cast<NestedLocation>(loc));
        else if(core::type_of<NestedSeqLocation>(loc))
            return execute(core::down_cast<NestedSeqLocation>(loc));
        else
            throw_bad_class(loc);
        return 0;
    }
    llvm::Value* execute(IdLocation& loc)
    {
        for(auto it : var_list)
        {
            auto value = it.value();
            auto name = value->getName().data();
            if(loc.id.equal(name))
            {
                llvm::IRBuilder<> builder(the_bb);
                return builder.CreateLoad(value, name);
            }
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
        switch(final.type)
        {
            case Type::INT8:
                return llvm::ConstantInt::get(llvm::Type::getInt8Ty(the_context), final.value.to_int8());
            case Type::INT16:
                return llvm::ConstantInt::get(llvm::Type::getInt16Ty(the_context), final.value.to_int16());
            case Type::INT32:
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_context), final.value.to_int32());
            case Type::INT64:
                return llvm::ConstantInt::get(llvm::Type::getInt64Ty(the_context), final.value.to_int64());
            case Type::FLOAT32:
                return llvm::ConstantFP::get(llvm::Type::getFloatTy(the_context), final.value.to_float32());
            case Type::FLOAT64:
                return llvm::ConstantFP::get(llvm::Type::getDoubleTy(the_context), final.value.to_float64());
        }
    }
    llvm::Value* execute(llvm::Value* v1, Operator op, llvm::Value* v2)
    {
        llvm::IRBuilder<> builder(the_bb);

        auto t1 = v1->getType();
        auto t2 = v2->getType();

        if(t1->isFloatingPointTy() || t2->isFloatingPointTy())
        {
            if(t1->isIntegerTy())
                v1 = builder.CreateSIToFP(v1, t2);
            else if(t2->isIntegerTy())
                v2 = builder.CreateSIToFP(v2, t1);
            else if(t1->getPrimitiveSizeInBits() > t2->getPrimitiveSizeInBits())
                v2 = builder.CreateFPExt(v2, t1);
            else if(t2->getPrimitiveSizeInBits() > t1->getPrimitiveSizeInBits())
                v1 = builder.CreateFPExt(v1, t2);
            return execute_float(v1, op, v2);
        }
        else if(t1->isIntegerTy() && t2->isIntegerTy())
        {
            if(t1->getPrimitiveSizeInBits() > t2->getPrimitiveSizeInBits())
                v2 = builder.CreateSExt(v2, t1);
            else if(t2->getPrimitiveSizeInBits() > t1->getPrimitiveSizeInBits())
                v1 = builder.CreateSExt(v1, t2);
            return execute_int(v1, op, v2);
        }
        else
        {
            env::Throw::raise("Unknown type");
        }
        return 0;
    }
    llvm::Value* execute_int(llvm::Value* v1, Operator op, llvm::Value* v2)
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
                return builder.CreateICmpEQ(v1, v2);
            case Operator::NE:
                return builder.CreateICmpNE(v1, v2);
            case Operator::LT:
                return builder.CreateICmpSLT(v1, v2);
            case Operator::GT:
                return builder.CreateICmpSGT(v1, v2);
            case Operator::LE:
                return builder.CreateICmpSLE(v1, v2);
            case Operator::GE:
                return builder.CreateICmpSGE(v1, v2);
            case Operator::AND:
                return builder.CreateAnd(v1, v2);
            case Operator::OR:
                return builder.CreateOr(v1, v2);
            case Operator::XOR:
                return builder.CreateXor(v1, v2);
            case Operator::MOD:
            case Operator::NOT:
            default:
                env::Throw::raise("Unknown binary int operator");
        }
        return 0;
    }
    llvm::Value* execute_float(llvm::Value* v1, Operator op, llvm::Value* v2)
    {
        llvm::IRBuilder<> builder(the_bb);
        switch(op)
        {
            case Operator::MUL:
                return builder.CreateFMul(v1, v2);
            case Operator::DIV:
                return builder.CreateFDiv(v1, v2);
            case Operator::ADD:
                return builder.CreateFAdd(v1, v2);
            case Operator::SUB:
                return builder.CreateFSub(v1, v2);
            case Operator::SHL:
            case Operator::SHR:
            case Operator::EQ:
                return builder.CreateFCmpUEQ(v1, v2);
            case Operator::NE:
                return builder.CreateFCmpUNE(v1, v2);
            case Operator::LT:
                return builder.CreateFCmpULT(v1, v2);
            case Operator::GT:
                return builder.CreateFCmpUGT(v1, v2);
            case Operator::LE:
                return builder.CreateFCmpULE(v1, v2);
            case Operator::GE:
                return builder.CreateFCmpUGE(v1, v2);
            case Operator::AND:
            case Operator::OR:
            case Operator::XOR:
            case Operator::MOD:
            case Operator::NOT:
            default:
                env::Throw::raise("Unknown binary float operator");
        }
        return 0;
    }
    template<class Type>
    static void throw_bad_class(const Type& type)
    {
        env::Throw("Writable: $1 not handled")
            .arg(typeid(type).name())
            .end();
    }
private:
    llvm::LLVMContext the_context;
    std::unique_ptr<llvm::Module> the_module;
    llvm::Function* the_func;
    llvm::BasicBlock* the_bb;
    core::List<llvm::AllocaInst*> var_list;
};
