
class Writer
{
public:
    static void execute(Tree& tree, core::String& filename)
    {
        Writer writer;
        writer.execute_tree(tree, filename);
    }
private:
    struct Context
    {
        llvm::StructType* clazz_type;
        llvm::AllocaInst* clazz_alloca;
        llvm::BasicBlock* create_entry;
        llvm::BasicBlock* destroy_entry;
        std::vector<llvm::Type*> field_vec;
    };
private:
    Writer()
    {
        llvm::InitializeNativeTarget();
    }
    ~Writer()
    {
        llvm::llvm_shutdown();
    }
    void execute_tree(Tree& tree, core::String& filename)
    {
        // initialize
        the_module = llvm::make_unique<llvm::Module>(filename.ascii(), the_llvm);

        // malloc
        auto malloc_result = llvm::PointerType::get(llvm::Type::getInt8Ty(the_llvm), 0);
        llvm::Type* malloc_args[] = { llvm::Type::getInt64Ty(the_llvm) };
        auto malloc_type = llvm::FunctionType::get(malloc_result, malloc_args, false);
        the_malloc_func = llvm::Function::Create(malloc_type, llvm::GlobalValue::ExternalLinkage, "malloc", the_module.get());

        // free
        auto free_result = llvm::Type::getVoidTy(the_llvm);
        llvm::Type* free_args[] = { llvm::PointerType::get(llvm::Type::getInt8Ty(the_llvm), 0) };
        auto free_type = llvm::FunctionType::get(free_result, free_args, false);
        the_free_func = llvm::Function::Create(free_type, llvm::GlobalValue::ExternalLinkage, "free", the_module.get());

        // main
        auto main_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(the_llvm), {}, false);
        auto main_func = llvm::Function::Create(main_type, llvm::GlobalValue::ExternalLinkage, "main", the_module.get());
        auto main_create_entry = llvm::BasicBlock::Create(the_llvm, "create_entry", main_func);
        auto main_destroy_entry = llvm::BasicBlock::Create(the_llvm, "destroy_entry", main_func);

        // build body of main function
        Context context = { 0, 0, main_create_entry, main_destroy_entry, {} };
        execute(tree.var(), context);

        // create main function return
        auto main_return = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        llvm::ReturnInst::Create(the_llvm, main_return, main_create_entry);
        llvm::ReturnInst::Create(the_llvm, main_return, main_destroy_entry);

        // print and verify module
        llvm::outs() << "LLVM module:\n" << *the_module;
        llvm::verifyModule(*the_module, &llvm::outs());

        // call the generated main function
        auto engine = llvm::EngineBuilder(std::move(the_module)).create();
        auto ret = engine->runFunction(main_func, {});
        auto result = ret.IntVal.getSExtValue();
        delete engine;

        core::verify(!result);
    }
    void execute(Var& var, Context& context)
    {
        if(core::type_of<IdVar>(var))
            execute(core::down_cast<IdVar>(var), context);
        else if(core::type_of<AssignVar>(var))
            execute(core::down_cast<AssignVar>(var), context);
        else if(core::type_of<CompositeVar>(var))
            execute(core::down_cast<CompositeVar>(var), context);
        else
            throw bad_class_exception(var);
    }
    void execute(IdVar& var, Context& context)
    {
        core::certify(false);
    }
    void execute(AssignVar& var, Context& context)
    {
        auto const_int32_0 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        auto const_int32_i = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), context.field_vec.size());

        auto value = execute(var.exp, context);

        context.field_vec.push_back(value->getType());
        context.clazz_type->setBody(context.field_vec, false);

        auto clazz_load = new llvm::LoadInst(context.clazz_alloca, "", false, context.create_entry);
        auto clazz_field = llvm::GetElementPtrInst::Create(context.clazz_type,
                                                           clazz_load,
                                                           { const_int32_0, const_int32_i },
                                                           "gep",
                                                           context.create_entry);
        new llvm::StoreInst(value, clazz_field, false, context.create_entry);
    }
    void execute(CompositeVar& var, Context& context)
    {
        auto clazz_id = var_id(var.signature_var);

        // clazz
        auto clazz_type = llvm::StructType::create(the_llvm, clazz_name(clazz_id).ascii());
        auto clazz_type_ptr = llvm::PointerType::get(clazz_type, 0);

        // clazz alloca
        auto clazz_alloca = new llvm::AllocaInst(clazz_type_ptr, clazz_id.ascii(), context.create_entry);

        // clazz create/destroy
        auto create_type = llvm::FunctionType::get(clazz_type_ptr, {}, false);
        auto create_func = llvm::Function::Create(create_type,
                                                  llvm::GlobalValue::ExternalLinkage,
                                                  create_name(clazz_id).ascii(),
                                                  the_module.get());
        auto create_entry = llvm::BasicBlock::Create(the_llvm, "create_entry", create_func);
        auto destroy_type = llvm::FunctionType::get(llvm::Type::getVoidTy(the_llvm), { clazz_type_ptr }, false);
        auto destroy_func = llvm::Function::Create(destroy_type,
                                                   llvm::GlobalValue::ExternalLinkage,
                                                   destroy_name(clazz_id).ascii(),
                                                   the_module.get());
        auto destroy_entry = llvm::BasicBlock::Create(the_llvm, "destroy_entry", destroy_func);

        // call clazz and create/destroy by parent
        if(context.clazz_type)
        {
            context.field_vec.push_back(clazz_type);
            context.clazz_type->setBody(context.field_vec, false);
        }
        llvm::CallInst::Create(create_func, {}, "call", context.create_entry);
        llvm::CallInst::Create(destroy_func, {}, "call", context.destroy_entry);

        // clazz and create/destroy body
        Context in_context = { clazz_type, clazz_alloca, create_entry, destroy_entry, {} };
        for(auto& it : var.var_list)
        {
            auto field = it.value();
            execute(field, in_context);
        }

        // call malloc
        auto clazz_size = the_module->getDataLayout().getTypeAllocSize(clazz_type);
        auto clazz_size_const = llvm::ConstantInt::get(llvm::Type::getInt64Ty(the_llvm), clazz_size);
        auto call_malloc = llvm::CallInst::Create(the_malloc_func, clazz_size_const, "call", clazz_alloca);

        // store malloc result
        auto cast = new llvm::BitCastInst(call_malloc, clazz_type_ptr, "cast", create_entry);
        new llvm::StoreInst(cast, clazz_alloca, false, create_entry);
    }
    llvm::Value* execute(Expression& exp, Context& context)
    {
        if(core::type_of<FinalExpression>(exp))
            return execute(core::down_cast<FinalExpression>(exp), context);
        else if(core::type_of<LocationExpression>(exp))
            return execute(core::down_cast<LocationExpression>(exp), context);
        else if(core::type_of<NestFinalExpression>(exp))
            return execute(core::down_cast<NestFinalExpression>(exp), context);
        else if(core::type_of<NestLocationExpression>(exp))
            return execute(core::down_cast<NestLocationExpression>(exp), context);
        else if(core::type_of<NestExpression>(exp))
            return execute(core::down_cast<NestExpression>(exp), context);
        else
            throw bad_class_exception(exp);
    }
    llvm::Value* execute(FinalExpression& exp, Context& context)
    {
        return execute(exp.final);
    }
    llvm::Value* execute(LocationExpression& exp, Context& context)
    {
        return execute(exp.loc, context);
    }
    llvm::Value* execute(NestFinalExpression& exp, Context& context)
    {
        auto exp_val = execute(exp.exp, context);
        auto final_val = execute(exp.final);
        return execute(exp_val, exp.op, final_val, context);
    }
    llvm::Value* execute(NestLocationExpression& exp, Context& context)
    {
        auto exp_val = execute(exp.exp, context);
        auto loc_val = execute(exp.loc, context);
        return execute(exp_val, exp.op, loc_val, context);
    }
    llvm::Value* execute(NestExpression& exp, Context& context)
    {
        auto val1 = execute(exp.exp1, context);
        auto val2 = execute(exp.exp2, context);
        return execute(val1, exp.op, val2, context);
    }
    llvm::Value* execute(Location& loc, Context& context)
    {
        if(core::type_of<IdLocation>(loc))
            return execute(core::down_cast<IdLocation>(loc), context);
        else if(core::type_of<FilterLocation>(loc))
            return execute(core::down_cast<FilterLocation>(loc), context);
        else if(core::type_of<NestIdLocation>(loc))
            return execute(core::down_cast<NestIdLocation>(loc), context);
        else if(core::type_of<NestFilterLocation>(loc))
            return execute(core::down_cast<NestFilterLocation>(loc), context);
        else
            throw bad_class_exception(loc);
    }
    llvm::Value* execute(IdLocation& loc, Context& context)
    {
        core::List<llvm::AllocaInst*> list;
        for(auto it : list)
        {
            auto value = it.value();
            auto name = value->getName().data();
            if(loc.id.equal(name))
                return new llvm::LoadInst(value, name, false, context.create_entry);
        }
        throw env::Format("Unknown variable: $1").arg(loc.id).exception();
    }
    llvm::Value* execute(FilterLocation& loc)
    {
        core::certify(false);
        return 0;
    }
    llvm::Value* execute(NestIdLocation& loc)
    {
        core::certify(false);
        return 0;
    }
    llvm::Value* execute(NestFilterLocation& loc)
    {
        core::certify(false);
        return 0;
    }
    llvm::Value* execute(Final& final)
    {
        switch(final.type)
        {
            case Type::INT8:
                return llvm::ConstantInt::get(llvm::Type::getInt8Ty(the_llvm), final.value.to_int8());
            case Type::INT16:
                return llvm::ConstantInt::get(llvm::Type::getInt16Ty(the_llvm), final.value.to_int16());
            case Type::INT32:
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), final.value.to_int32());
            case Type::INT64:
                return llvm::ConstantInt::get(llvm::Type::getInt64Ty(the_llvm), final.value.to_int64());
            case Type::FLOAT32:
                return llvm::ConstantFP::get(llvm::Type::getFloatTy(the_llvm), final.value.to_float32());
            case Type::FLOAT64:
                return llvm::ConstantFP::get(llvm::Type::getDoubleTy(the_llvm), final.value.to_float64());
        }
    }
    llvm::Value* execute(llvm::Value* val1, BinaryOp op, llvm::Value* val2, Context& context)
    {
        auto type1 = val1->getType();
        auto type2 = val2->getType();

        if(type1->isFloatingPointTy() || type2->isFloatingPointTy())
        {
            if(type1->isIntegerTy())
                val1 = new llvm::SIToFPInst(val1, type2, "sitofp", context.create_entry);
            else if(type2->isIntegerTy())
                val2 = new llvm::SIToFPInst(val2, type1, "sitofp", context.create_entry);
            else if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::FPExtInst(val2, type1, "fpext", context.create_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::FPExtInst(val1, type2, "fpext", context.create_entry);
            return execute_float(val1, op, val2, context);
        }
        else if(type1->isIntegerTy() && type2->isIntegerTy())
        {
            if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::SExtInst(val2, type1, "sext", context.create_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::SExtInst(val1, type2, "sext", context.create_entry);
            return execute_int(val1, op, val2, context);
        }
        else
        {
            throw env::Format("Cannot handle llvm::Type::TypeID's: [$1, $2]")
                .arg(type1->getTypeID())
                .arg(type2->getTypeID())
                .exception();
        }
    }
    llvm::Value* execute_int(llvm::Value* val1, BinaryOp op, llvm::Value* val2, Context& context)
    {
        switch(op)
        {
            case BinaryOp::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Mul, val1, val2, "mul", context.create_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, val1, val2, "div", context.create_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::Add, val1, val2, "add", context.create_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, val1, val2, "sub", context.create_entry);
            case BinaryOp::SHL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Shl, val1, val2, "shl", context.create_entry);
            case BinaryOp::SHR:
                return llvm::BinaryOperator::Create(llvm::Instruction::AShr, val1, val2, "shr", context.create_entry);
            case BinaryOp::EQ:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_EQ, val1, val2, "eq");
            case BinaryOp::NE:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_NE, val1, val2, "ne");
            case BinaryOp::LT:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SLT, val1, val2, "lt");
            case BinaryOp::GT:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SGT, val1, val2, "qt");
            case BinaryOp::LE:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SLE, val1, val2, "le");
            case BinaryOp::GE:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SGE, val1, val2, "ge");
            case BinaryOp::AND:
                return llvm::BinaryOperator::Create(llvm::Instruction::And, val1, val2, "and", context.create_entry);
            case BinaryOp::OR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, val1, val2, "or", context.create_entry);
            case BinaryOp::XOR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Xor, val1, val2, "xor", context.create_entry);
            case BinaryOp::MOD:
                return llvm::BinaryOperator::Create(llvm::Instruction::SRem, val1, val2, "rem", context.create_entry);
        }
        return 0;
    }
    llvm::Value* execute_float(llvm::Value* val1, BinaryOp op, llvm::Value* val2, Context& context)
    {
        switch(op)
        {
            case BinaryOp::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::FMul, val1, val2, "fmul", context.create_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::FDiv, val1, val2, "fdiv", context.create_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::FAdd, val1, val2, "fadd", context.create_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::FSub, val1, val2, "fsub", context.create_entry);
            case BinaryOp::EQ:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OEQ, val1, val2, "eq");
            case BinaryOp::NE:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_UNE, val1, val2, "ne");
            case BinaryOp::LT:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OLT, val1, val2, "lt");
            case BinaryOp::GT:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OGT, val1, val2, "gt");
            case BinaryOp::LE:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OLE, val1, val2, "le");
            case BinaryOp::GE:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OGE, val1, val2, "ge");
            case BinaryOp::SHL:
            case BinaryOp::SHR:
            case BinaryOp::AND:
            case BinaryOp::OR:
            case BinaryOp::XOR:
            case BinaryOp::MOD:
                throw env::Format("Binary operator '$1' not supported for floating point")
                    .arg(binary_op_name(op))
                    .exception();
        }
    }
    core::String clazz_name(core::String& clazz_id)
    {
        return core::Format("$1_clazz").arg(clazz_id).end();
    }
    core::String create_name(core::String& clazz_id)
    {
        return core::Format("$1_create").arg(clazz_id).end();
    }
    core::String destroy_name(core::String& clazz_id)
    {
        return core::Format("$1_destroy").arg(clazz_id).end();
    }
    template<class Type>
    env::Exception bad_class_exception(Type& var)
    {
        return env::Format("Writable: $1 not handled")
            .arg(typeid(var).name())
            .exception();
    }
    core::String var_id(Var& var)
    {
        if(core::type_of<IdVar>(var))
            return core::down_cast<IdVar>(var).id;
        else if(core::type_of<AssignVar>(var))
            return core::down_cast<AssignVar>(var).id;
        else
            core::certify(false);
        return core::String();
    }
private:
    llvm::Function* the_malloc_func;
    llvm::Function* the_free_func;
    llvm::LLVMContext the_llvm;
    std::unique_ptr<llvm::Module> the_module;
};
