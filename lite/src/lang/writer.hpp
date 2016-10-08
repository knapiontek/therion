
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
        // initialize
        the_module = llvm::make_unique<llvm::Module>("test", the_context);
        auto type = llvm::FunctionType::get(llvm::Type::getInt32Ty(the_context), {}, false);
        auto function = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, "main", the_module.get());
        the_entry = llvm::BasicBlock::Create(the_context, "entry", function);

        // build body of main function
        for(auto& it : tree.var_list())
        {
            auto value = execute(it.value());
            var_list.append(value);
        }

        // create function return
        auto var = var_list[var_list.size() - 1];
        auto var_load = new llvm::LoadInst(var, var->getName(), false, the_entry);
        llvm::ReturnInst::Create(the_context, var_load, the_entry);

        // print and verify module
        llvm::outs() << "LLVM module:\n" << *the_module;
        llvm::verifyModule(*the_module, &llvm::outs());

        // call generated main function
        auto engine = llvm::EngineBuilder(std::move(the_module)).create();
        auto ret = engine->runFunction(function, {});
        auto result = ret.IntVal.getSExtValue();
        delete engine;

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
        auto value = execute(var.expression);
        auto alloca = new llvm::AllocaInst(value->getType(), var.id.ascii(), the_entry);
        new llvm::StoreInst(value, alloca, false, the_entry);
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
                return new llvm::LoadInst(value, name, false, the_entry);
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
        auto t1 = v1->getType();
        auto t2 = v2->getType();

        if(t1->isFloatingPointTy() || t2->isFloatingPointTy())
        {
            if(t1->isIntegerTy())
                v1 = new llvm::SIToFPInst(v1, t2, "sitofp", the_entry);
            else if(t2->isIntegerTy())
                v2 = new llvm::SIToFPInst(v2, t1, "sitofp", the_entry);
            else if(t1->getPrimitiveSizeInBits() > t2->getPrimitiveSizeInBits())
                v2 = new llvm::FPExtInst(v2, t1, "fpext", the_entry);
            else if(t2->getPrimitiveSizeInBits() > t1->getPrimitiveSizeInBits())
                v1 = new llvm::FPExtInst(v1, t2, "fpext", the_entry);
            return execute_float(v1, op, v2);
        }
        else if(t1->isIntegerTy() && t2->isIntegerTy())
        {
            if(t1->getPrimitiveSizeInBits() > t2->getPrimitiveSizeInBits())
                v2 = new llvm::SExtInst(v2, t1, "sext", the_entry);
            else if(t2->getPrimitiveSizeInBits() > t1->getPrimitiveSizeInBits())
                v1 = new llvm::SExtInst(v1, t2, "sext", the_entry);
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
        switch(op)
        {
            case Operator::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Mul, v1, v2, "mul", the_entry);
            case Operator::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::UDiv, v1, v2, "udiv", the_entry);
            case Operator::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::Add, v1, v2, "add", the_entry);
            case Operator::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, v1, v2, "sub", the_entry);
            case Operator::SHL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Shl, v1, v2, "shl", the_entry);
            case Operator::SHR:
                return llvm::BinaryOperator::Create(llvm::Instruction::LShr, v1, v2, "lshr", the_entry);
            case Operator::EQ:
            case Operator::NE:
            case Operator::LT:
            case Operator::GT:
            case Operator::LE:
            case Operator::GE:
            case Operator::AND:
                return llvm::BinaryOperator::Create(llvm::Instruction::And, v1, v2, "and", the_entry);
            case Operator::OR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, v1, v2, "or", the_entry);
            case Operator::XOR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Xor, v1, v2, "xor", the_entry);
            case Operator::MOD:
            case Operator::NOT:
            default:
                env::Throw::raise("Unknown binary int operator");
        }
        return 0;
    }
    llvm::Value* execute_float(llvm::Value* v1, Operator op, llvm::Value* v2)
    {
        switch(op)
        {
            case Operator::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::FMul, v1, v2, "fmul", the_entry);
            case Operator::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::FDiv, v1, v2, "fdiv", the_entry);
            case Operator::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::FAdd, v1, v2, "fadd", the_entry);
            case Operator::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::FSub, v1, v2, "fsub", the_entry);
            case Operator::SHL:
            case Operator::SHR:
            case Operator::EQ:
            case Operator::NE:
            case Operator::LT:
            case Operator::GT:
            case Operator::LE:
            case Operator::GE:
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
    llvm::BasicBlock* the_entry;
    std::unique_ptr<llvm::Module> the_module;
    llvm::LLVMContext the_context;
    core::List<llvm::AllocaInst*> var_list;
};
