
class Writer
{
public:
    static void execute(Tree& tree)
    {
        Writer writer;
        writer.execute_tree(tree);
    }
private:
    Writer() : the_var_list(0xA)
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

        // main
        auto main_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(the_context), {}, false);
        auto main = llvm::Function::Create(main_type, llvm::GlobalValue::ExternalLinkage, "main", the_module.get());
        the_entry = llvm::BasicBlock::Create(the_context, "entry", main);

        // malloc
        auto malloc_result = llvm::PointerType::get(llvm::Type::getInt8Ty(the_context), 0);
        llvm::Type* malloc_args[] = { llvm::Type::getInt64Ty(the_context) };
        auto malloc_type = llvm::FunctionType::get(malloc_result, malloc_args, false);
        llvm::Function::Create(malloc_type, llvm::GlobalValue::ExternalLinkage, "malloc", the_module.get());

        // build body of main function
        for(auto& it : tree.var_list())
        {
            auto value = execute(it.value());
            the_var_list.append(value);
        }

        // create function return
        auto var = the_var_list[the_var_list.size() - 1];
        auto var_load = new llvm::LoadInst(var, var->getName(), false, the_entry);
        llvm::ReturnInst::Create(the_context, var_load, the_entry);

        // print and verify module
        llvm::outs() << "LLVM module:\n" << *the_module;
        llvm::verifyModule(*the_module, &llvm::outs());

        // call generated main function
        auto engine = llvm::EngineBuilder(std::move(the_module)).create();
        auto ret = engine->runFunction(main, {});
        auto result = ret.IntVal.getSExtValue();
        delete engine;

        core::verify(!result);
    }
    llvm::AllocaInst* execute(Var& var)
    {
        if(core::type_of<SingleVar>(var))
            return execute(core::down_cast<SingleVar>(var));
        else if(core::type_of<CompositeVar>(var))
            return execute(core::down_cast<CompositeVar>(var));
        else
            throw_bad_class(var);
        return 0;
    }
    llvm::AllocaInst* execute(SingleVar& var)
    {
        auto value = execute(var.exp);
        auto alloca = new llvm::AllocaInst(value->getType(), var.id.ascii(), the_entry);
        new llvm::StoreInst(value, alloca, false, the_entry);
        return alloca;
    }
    llvm::AllocaInst* execute(CompositeVar& var)
    {
        core::verify(var.var.type_of<SingleVar>());
        auto& single = var.var.down_cast<SingleVar>();

        // create struct
        auto struct_name = core::Format("$1_struct").arg(single.id).end();
        auto struct_type = llvm::StructType::create(the_context, struct_name.ascii());
        std::vector<llvm::Type*> fields;
        for(auto& it : var.var_list)
        {
            auto field = it.value();
            core::verify(field.type_of<SingleVar>());
            auto& single = field.down_cast<SingleVar>();
            auto field_exp = execute(single.exp);
            fields.push_back(field_exp->getType());
        }
        struct_type->setBody(fields, false);

        // alloca struct pointer
        auto struct_ptr_type = llvm::PointerType::get(struct_type, 0);
        auto struct_ptr = new llvm::AllocaInst(struct_ptr_type, single.id.ascii(), the_entry);

        // call malloc
        auto struct_size = the_module->getDataLayout().getTypeAllocSize(struct_type);
        auto struct_size_const = llvm::ConstantInt::get(llvm::Type::getInt64Ty(the_context), struct_size);
        auto malloc = the_module->getFunction("malloc");
        auto call_malloc = llvm::CallInst::Create(malloc, struct_size_const, "", the_entry);

        // store result
        auto cast = new llvm::BitCastInst(call_malloc, struct_ptr_type, "", the_entry);
        new llvm::StoreInst(cast, struct_ptr, false, the_entry);

        return struct_ptr;
    }
    llvm::Value* execute(Expression& exp)
    {
        if(core::type_of<FinalExpression>(exp))
            return execute(core::down_cast<FinalExpression>(exp));
        else if(core::type_of<LocationExpression>(exp))
            return execute(core::down_cast<LocationExpression>(exp));
        else if(core::type_of<NestFinalExpression>(exp))
            return execute(core::down_cast<NestFinalExpression>(exp));
        else if(core::type_of<NestLocationExpression>(exp))
            return execute(core::down_cast<NestLocationExpression>(exp));
        else if(core::type_of<NestExpression>(exp))
            return execute(core::down_cast<NestExpression>(exp));
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
    llvm::Value* execute(NestFinalExpression& exp)
    {
        auto exp_val = execute(exp.exp);
        auto final_val = execute(exp.final);
        return execute(exp_val, exp.op, final_val);
    }
    llvm::Value* execute(NestLocationExpression& exp)
    {
        auto exp_val = execute(exp.exp);
        auto loc_val = execute(exp.loc);
        return execute(exp_val, exp.op, loc_val);
    }
    llvm::Value* execute(NestExpression& exp)
    {
        auto val1 = execute(exp.exp1);
        auto val2 = execute(exp.exp2);
        return execute(val1, exp.op, val2);
    }
    llvm::Value* execute(Location& loc)
    {
        if(core::type_of<IdLocation>(loc))
            return execute(core::down_cast<IdLocation>(loc));
        else if(core::type_of<FilterLocation>(loc))
            return execute(core::down_cast<FilterLocation>(loc));
        else if(core::type_of<NestIdLocation>(loc))
            return execute(core::down_cast<NestIdLocation>(loc));
        else if(core::type_of<NestFilterLocation>(loc))
            return execute(core::down_cast<NestFilterLocation>(loc));
        else
            throw_bad_class(loc);
        return 0;
    }
    llvm::Value* execute(IdLocation& loc)
    {
        for(auto it : the_var_list)
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
    llvm::Value* execute(FilterLocation& loc)
    {
        core::verify(false);
        return 0;
    }
    llvm::Value* execute(NestIdLocation& loc)
    {
        core::verify(false);
        return 0;
    }
    llvm::Value* execute(NestFilterLocation& loc)
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
    llvm::Value* execute(llvm::Value* val1, BinaryOp op, llvm::Value* val2)
    {
        auto type1 = val1->getType();
        auto type2 = val2->getType();

        if(type1->isFloatingPointTy() || type2->isFloatingPointTy())
        {
            if(type1->isIntegerTy())
                val1 = new llvm::SIToFPInst(val1, type2, "sitofp", the_entry);
            else if(type2->isIntegerTy())
                val2 = new llvm::SIToFPInst(val2, type1, "sitofp", the_entry);
            else if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::FPExtInst(val2, type1, "fpext", the_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::FPExtInst(val1, type2, "fpext", the_entry);
            return execute_float(val1, op, val2);
        }
        else if(type1->isIntegerTy() && type2->isIntegerTy())
        {
            if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::SExtInst(val2, type1, "sext", the_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::SExtInst(val1, type2, "sext", the_entry);
            return execute_int(val1, op, val2);
        }
        else
        {
            env::Throw::raise("Unknown type");
        }

        return 0;
    }
    llvm::Value* execute_int(llvm::Value* val1, BinaryOp op, llvm::Value* val2)
    {
        switch(op)
        {
            case BinaryOp::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Mul, val1, val2, "mul", the_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::UDiv, val1, val2, "udiv", the_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::Add, val1, val2, "add", the_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, val1, val2, "sub", the_entry);
            case BinaryOp::SHL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Shl, val1, val2, "shl", the_entry);
            case BinaryOp::SHR:
                return llvm::BinaryOperator::Create(llvm::Instruction::LShr, val1, val2, "lshr", the_entry);
            case BinaryOp::EQ:
            case BinaryOp::NE:
            case BinaryOp::LT:
            case BinaryOp::GT:
            case BinaryOp::LE:
            case BinaryOp::GE:
            case BinaryOp::AND:
                return llvm::BinaryOperator::Create(llvm::Instruction::And, val1, val2, "and", the_entry);
            case BinaryOp::OR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, val1, val2, "or", the_entry);
            case BinaryOp::XOR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Xor, val1, val2, "xor", the_entry);
            case BinaryOp::MOD:
            default:
                env::Throw::raise("Unknown binary int operator");
        }
        return 0;
    }
    llvm::Value* execute_float(llvm::Value* val1, BinaryOp op, llvm::Value* val2)
    {
        switch(op)
        {
            case BinaryOp::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::FMul, val1, val2, "fmul", the_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::FDiv, val1, val2, "fdiv", the_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::FAdd, val1, val2, "fadd", the_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::FSub, val1, val2, "fsub", the_entry);
            case BinaryOp::SHL:
            case BinaryOp::SHR:
            case BinaryOp::EQ:
            case BinaryOp::NE:
            case BinaryOp::LT:
            case BinaryOp::GT:
            case BinaryOp::LE:
            case BinaryOp::GE:
            case BinaryOp::AND:
            case BinaryOp::OR:
            case BinaryOp::XOR:
            case BinaryOp::MOD:
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
    llvm::LLVMContext the_context;
    std::unique_ptr<llvm::Module> the_module;
    core::List<llvm::AllocaInst*> the_var_list;
};
