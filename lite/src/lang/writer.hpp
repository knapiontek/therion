
class Writer
{
public:
    static void execute(Tree& tree, core::String& filename)
    {
        Writer writer(filename);
        writer.execute_main(tree);
    }
private:
    struct Context
    {
        llvm::StructType* clazz_type;
        llvm::AllocaInst* clazz_ptr_alloca;
        llvm::BasicBlock* create_entry;
        llvm::BasicBlock* destroy_entry;
        std::vector<llvm::Type*> field_vec;
    };
private:
    Writer(core::String& filename)
    {
        // initialize
        llvm::InitializeNativeTarget();
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
    }
    ~Writer()
    {
        llvm::llvm_shutdown();
    }
    void execute_main(Tree& tree)
    {
        // main
        auto clazz_type = llvm::StructType::create(the_llvm, "main_clazz");
        auto clazz_ptr_type = llvm::PointerType::get(clazz_type, 0);
        auto func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(the_llvm), {}, false);
        auto func = llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage, "main", the_module.get());
        auto create_entry = llvm::BasicBlock::Create(the_llvm, "create_entry", func);
        auto destroy_entry = llvm::BasicBlock::Create(the_llvm, "destroy_entry", func);

        // main body
        auto clazz_alloca = new llvm::AllocaInst(clazz_type, nil, create_entry);
        auto clazz_ptr_alloca = new llvm::AllocaInst(clazz_ptr_type, nil, create_entry);
        new llvm::StoreInst(clazz_alloca, clazz_ptr_alloca, false, create_entry);
        Context context = { clazz_type, clazz_ptr_alloca, create_entry, destroy_entry, {} };
        for(auto& it : tree.var().var_list)
        {
            auto field = it.value();
            execute(field, context);
        }

        // main finish
        llvm::BranchInst::Create(destroy_entry, create_entry);
        auto return_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        llvm::ReturnInst::Create(the_llvm, return_const, destroy_entry);

        // print and verify module
        llvm::outs() << "LLVM module:\n" << *the_module;
        llvm::verifyModule(*the_module, &llvm::outs());

        // call main
        auto engine = llvm::EngineBuilder(std::move(the_module)).create();
        auto ret = engine->runFunction(func, {});
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
    void append_new_field(llvm::Type* type, Context& context)
    {
        context.field_vec.push_back(type);
        context.clazz_type->setBody(context.field_vec, false);
    }
    llvm::Value* load_last_field(Context& context)
    {
        auto int32_0_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        auto int32_i_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), context.field_vec.size() - 1);
        auto clazz_load = new llvm::LoadInst(context.clazz_ptr_alloca, nil, false, context.destroy_entry);
        auto clazz_field = llvm::GetElementPtrInst::Create(context.clazz_type,
                                                           clazz_load,
                                                           { int32_0_const, int32_i_const },
                                                           nil,
                                                           context.destroy_entry);
        return new llvm::LoadInst(clazz_field, nil, false, context.destroy_entry);
    }
    void store_last_field(llvm::Value* value, Context& context)
    {
        auto int32_0_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        auto int32_i_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), context.field_vec.size() - 1);
        auto clazz_load = new llvm::LoadInst(context.clazz_ptr_alloca, nil, false, context.create_entry);
        auto clazz_field = llvm::GetElementPtrInst::Create(context.clazz_type,
                                                           clazz_load,
                                                           { int32_0_const, int32_i_const },
                                                           nil,
                                                           context.create_entry);
        new llvm::StoreInst(value, clazz_field, false, context.create_entry);
    }
    void execute(AssignVar& var, Context& context)
    {
        auto exp_val = execute(var.exp, context);
        append_new_field(exp_val->getType(), context);
        store_last_field(exp_val, context);
    }
    void execute(CompositeVar& var, Context& context)
    {
        auto clazz_id = var_id(var.signature_var);

        // clazz
        auto clazz_type = llvm::StructType::create(the_llvm, clazz_name(clazz_id).ascii());
        auto clazz_type_ptr = llvm::PointerType::get(clazz_type, 0);

        // clazz create/destroy
        auto create_func_type = llvm::FunctionType::get(clazz_type_ptr, {}, false);
        auto destroy_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(the_llvm), { clazz_type_ptr }, false);
        auto create_func = llvm::Function::Create(create_func_type,
                                                  llvm::GlobalValue::ExternalLinkage,
                                                  create_name(clazz_id).ascii(),
                                                  the_module.get());
        auto destroy_func = llvm::Function::Create(destroy_func_type,
                                                   llvm::GlobalValue::ExternalLinkage,
                                                   destroy_name(clazz_id).ascii(),
                                                   the_module.get());
        auto create_entry = llvm::BasicBlock::Create(the_llvm, "create_entry", create_func);
        auto destroy_entry = llvm::BasicBlock::Create(the_llvm, "destroy_entry", destroy_func);

        // call from parent
        append_new_field(clazz_type_ptr, context);
        auto create_call = llvm::CallInst::Create(create_func, {}, nil, context.create_entry);
        store_last_field(create_call, context);
        auto clazz_field = load_last_field(context);
        llvm::CallInst::Create(destroy_func, { clazz_field }, nil, context.destroy_entry);

        // clazz and create/destroy body
        auto clazz_ptr_alloca = new llvm::AllocaInst(clazz_type_ptr, nil, create_entry);
        Context in_context = { clazz_type, clazz_ptr_alloca, create_entry, destroy_entry, {} };
        auto start_body = new llvm::AllocaInst(llvm::Type::getInt8Ty(the_llvm), "start_body", create_entry);
        for(auto& it : var.var_list)
        {
            auto field = it.value();
            execute(field, in_context);
        }

        // deferred call malloc
        auto clazz_size = the_module->getDataLayout().getTypeAllocSize(clazz_type);
        auto clazz_size_const = llvm::ConstantInt::get(llvm::Type::getInt64Ty(the_llvm), clazz_size);
        auto call_malloc = llvm::CallInst::Create(the_malloc_func, clazz_size_const, nil, start_body);
        auto bit_cast1 = new llvm::BitCastInst(call_malloc, clazz_type_ptr, nil, start_body);
        new llvm::StoreInst(bit_cast1, clazz_ptr_alloca, false, start_body);

        // call free
        auto clazz_load3 = new llvm::LoadInst(context.clazz_ptr_alloca, nil, false, destroy_entry);
        auto int8_ptr_type = llvm::PointerType::get(llvm::Type::getInt8Ty(the_llvm), 0);
        auto bit_cast2 = new llvm::BitCastInst(clazz_load3, int8_ptr_type, nil, destroy_entry);
        llvm::CallInst::Create(the_free_func, bit_cast2, nil, destroy_entry);

        // create return
        llvm::ReturnInst::Create(the_llvm, bit_cast1, create_entry);
        llvm::ReturnInst::Create(the_llvm, destroy_entry);
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
                val1 = new llvm::SIToFPInst(val1, type2, nil, context.create_entry);
            else if(type2->isIntegerTy())
                val2 = new llvm::SIToFPInst(val2, type1, nil, context.create_entry);
            else if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::FPExtInst(val2, type1, nil, context.create_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::FPExtInst(val1, type2, nil, context.create_entry);
            return execute_float(val1, op, val2, context);
        }
        else if(type1->isIntegerTy() && type2->isIntegerTy())
        {
            if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::SExtInst(val2, type1, nil, context.create_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::SExtInst(val1, type2, nil, context.create_entry);
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
                return llvm::BinaryOperator::Create(llvm::Instruction::Mul, val1, val2, nil, context.create_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, val1, val2, nil, context.create_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::Add, val1, val2, nil, context.create_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, val1, val2, nil, context.create_entry);
            case BinaryOp::SHL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Shl, val1, val2, nil, context.create_entry);
            case BinaryOp::SHR:
                return llvm::BinaryOperator::Create(llvm::Instruction::AShr, val1, val2, nil, context.create_entry);
            case BinaryOp::EQ:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_EQ, val1, val2, nil);
            case BinaryOp::NE:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_NE, val1, val2, nil);
            case BinaryOp::LT:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SLT, val1, val2, nil);
            case BinaryOp::GT:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SGT, val1, val2, nil);
            case BinaryOp::LE:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SLE, val1, val2, nil);
            case BinaryOp::GE:
                return new llvm::ICmpInst(*context.create_entry, llvm::ICmpInst::ICMP_SGE, val1, val2, nil);
            case BinaryOp::AND:
                return llvm::BinaryOperator::Create(llvm::Instruction::And, val1, val2, nil, context.create_entry);
            case BinaryOp::OR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, val1, val2, nil, context.create_entry);
            case BinaryOp::XOR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Xor, val1, val2, nil, context.create_entry);
            case BinaryOp::MOD:
                return llvm::BinaryOperator::Create(llvm::Instruction::SRem, val1, val2, nil, context.create_entry);
        }
        return 0;
    }
    llvm::Value* execute_float(llvm::Value* val1, BinaryOp op, llvm::Value* val2, Context& context)
    {
        switch(op)
        {
            case BinaryOp::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::FMul, val1, val2, nil, context.create_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::FDiv, val1, val2, nil, context.create_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::FAdd, val1, val2, nil, context.create_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::FSub, val1, val2, nil, context.create_entry);
            case BinaryOp::EQ:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OEQ, val1, val2, nil);
            case BinaryOp::NE:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_UNE, val1, val2, nil);
            case BinaryOp::LT:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OLT, val1, val2, nil);
            case BinaryOp::GT:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OGT, val1, val2, nil);
            case BinaryOp::LE:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OLE, val1, val2, nil);
            case BinaryOp::GE:
                return new llvm::FCmpInst(*context.create_entry, llvm::FCmpInst::FCMP_OGE, val1, val2, nil);
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
        return core::Format("create_$1").arg(clazz_id).end();
    }
    core::String destroy_name(core::String& clazz_id)
    {
        return core::Format("destroy_$1").arg(clazz_id).end();
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
    const char* nil = "";
};
