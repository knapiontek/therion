
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
        core::Shared<Context> outer;
        core::Shared<ClazzVar> var;
        llvm::StructType* clazz_type;
        llvm::PointerType* clazz_ptr_type;
        llvm::AllocaInst* ctor_alloca;
        llvm::AllocaInst* dtor_alloca;
        llvm::BasicBlock* ctor_entry;
        llvm::BasicBlock* dtor_entry;
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
        auto ctor_begin = llvm::BasicBlock::Create(the_llvm, "ctor_begin", func);
        auto dtor_end = llvm::BasicBlock::Create(the_llvm, "dtor_end", func);

        // main context
        auto clazz_alloca = new llvm::AllocaInst(clazz_type, nil, ctor_begin);
        auto clazz_ptr_alloca = new llvm::AllocaInst(clazz_ptr_type, nil, ctor_begin);
        new llvm::StoreInst(clazz_alloca, clazz_ptr_alloca, false, ctor_begin);
        Context context = { core::nil, tree.main_var(),
                            clazz_type, clazz_ptr_type,
                            clazz_ptr_alloca, clazz_ptr_alloca,
                            ctor_begin, dtor_end, {} };

        // main body
        for(auto& field_var_it : tree.main_var().field_var_list)
        {
            auto field_var = field_var_it.value();
            execute(field_var, context);
        }

        // main finish
        llvm::BranchInst::Create(context.dtor_entry, context.ctor_entry);
        auto return_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        llvm::ReturnInst::Create(the_llvm, return_const, dtor_end);

        // print and verify module
        llvm::outs() << "LLVM module:\n" << *the_module;
        llvm::verifyModule(*the_module, &llvm::outs());

        // call main
        auto engine = llvm::EngineBuilder(std::move(the_module)).create();
        auto func_return = engine->runFunction(func, {});
        auto result = func_return.IntVal.getSExtValue();
        delete engine;

        core::verify(!result);
    }
    void execute(Var& var, Context& context)
    {
        if(core::type_of<IdVar>(var))
            execute(core::down_cast<IdVar>(var), context);
        else if(core::type_of<AssignVar>(var))
            execute(core::down_cast<AssignVar>(var), context);
        else if(core::type_of<ClazzVar>(var))
            execute(core::down_cast<ClazzVar>(var), context);
        else
            throw bad_class_exception(var);
    }
    void execute(IdVar& var, Context& context)
    {
        core::certify(false);
    }
    void execute(AssignVar& var, Context& context)
    {
        {
            // begin entry
            auto context_ctor_entry = llvm::BasicBlock::Create(the_llvm, var.id.ascii(), context.ctor_entry->getParent());
            context_ctor_entry->moveAfter(context.ctor_entry);
            core::xchange(context_ctor_entry, context.ctor_entry);

            // expression
            auto exp_val = execute(var.exp, context);

            // new field
            context.field_vec.push_back(exp_val->getType());
            context.clazz_type->setBody(context.field_vec, false);

            // store expression result in a new field
            auto last_field_pos = context.field_vec.size() - 1;
            auto clazz_load = new llvm::LoadInst(context.ctor_alloca, nil, false, context.ctor_entry);
            auto clazz_field = get_clazz_field(clazz_load, last_field_pos, context.ctor_entry);
            new llvm::StoreInst(exp_val, clazz_field, false, context.ctor_entry);

            // end entry
            llvm::BranchInst::Create(context.ctor_entry, context_ctor_entry);
        }

        {
            // empty entry
            auto context_dtor_entry = llvm::BasicBlock::Create(the_llvm, var.id.ascii(), context.dtor_entry->getParent(), context.dtor_entry);
            core::xchange(context_dtor_entry, context.dtor_entry);
            llvm::BranchInst::Create(context_dtor_entry, context.dtor_entry);
        }
    }
    void execute(ClazzVar& var, Context& context)
    {
        auto var_id = var.get_id();

        // clazz
        auto clazz_name = core::Format("%1_clazz") % var_id % core::end;
        auto clazz_type = llvm::StructType::create(the_llvm, clazz_name.ascii());
        auto clazz_ptr_type = llvm::PointerType::get(clazz_type, 0);
        context.field_vec.push_back(clazz_ptr_type);
        context.clazz_type->setBody(context.field_vec, false); // update StructType for GEP

        // ctor
        auto ctor_name = core::Format("ctor_%1") % var_id % core::end;
        auto ctor_func_type = llvm::FunctionType::get(clazz_ptr_type, {context.clazz_ptr_type}, false);
        auto ctor_func = llvm::Function::Create(ctor_func_type,
                                                llvm::GlobalValue::ExternalLinkage,
                                                ctor_name.ascii(),
                                                the_module.get());
        auto ctor_begin = llvm::BasicBlock::Create(the_llvm, "ctor_begin", ctor_func);
        auto ctor_end = llvm::BasicBlock::Create(the_llvm, "ctor_end", ctor_func);
        auto ctor_alloca = new llvm::AllocaInst(clazz_ptr_type, nil, ctor_begin);
        auto ctor_start = new llvm::AllocaInst(llvm::Type::getInt8Ty(the_llvm), nil, ctor_begin);

        // dtor
        auto dtor_name = core::Format("dtor_%1") % var_id % core::end;
        auto dtor_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(the_llvm), {clazz_ptr_type}, false);
        auto dtor_func = llvm::Function::Create(dtor_func_type,
                                                llvm::GlobalValue::ExternalLinkage,
                                                dtor_name.ascii(),
                                                the_module.get());
        auto dtor_begin = llvm::BasicBlock::Create(the_llvm, "dtor_begin", dtor_func);
        auto dtor_end = llvm::BasicBlock::Create(the_llvm, "dtor_end", dtor_func);
        auto dtor_alloca = new llvm::AllocaInst(clazz_ptr_type, nil, dtor_begin);
        auto dtor_start = new llvm::AllocaInst(llvm::Type::getInt8Ty(the_llvm), nil, dtor_begin);

        {
            // context ctor call
            auto context_ctor_entry = llvm::BasicBlock::Create(the_llvm, ctor_name.ascii(), context.ctor_entry->getParent());
            context_ctor_entry->moveAfter(context.ctor_entry);
            core::xchange(context_ctor_entry, context.ctor_entry);
            auto context_load = new llvm::LoadInst(context.ctor_alloca, nil, false, context.ctor_entry);
            auto ctor_call = llvm::CallInst::Create(ctor_func, {context_load}, nil, context.ctor_entry);
            auto last_field_pos = context.field_vec.size() - 1;
            auto clazz_load = new llvm::LoadInst(context.ctor_alloca, nil, false, context.ctor_entry);
            auto field = get_clazz_field(clazz_load, last_field_pos, context.ctor_entry);
            new llvm::StoreInst(ctor_call, field, false, context.ctor_entry);
            llvm::BranchInst::Create(context.ctor_entry, context_ctor_entry);
        }

        {
            // context dtor call
            auto context_dtor_entry = llvm::BasicBlock::Create(the_llvm, dtor_name.ascii(), context.dtor_entry->getParent(), context.dtor_entry);
            core::xchange(context_dtor_entry, context.dtor_entry);
            auto last_field_pos = context.field_vec.size() - 1;
            auto clazz_load = new llvm::LoadInst(context.dtor_alloca, nil, false, context.dtor_entry);
            auto field = get_clazz_field(clazz_load, last_field_pos, context.dtor_entry);
            auto clazz_field = new llvm::LoadInst(field, nil, false, context.dtor_entry);
            llvm::CallInst::Create(dtor_func, {clazz_field}, nil, context.dtor_entry);
            llvm::BranchInst::Create(context_dtor_entry, context.dtor_entry);
        }

        // context for clazz and ctor/dtor body
        Context in_context = { context, var,
                               clazz_type, clazz_ptr_type,
                               ctor_alloca, dtor_alloca,
                               ctor_begin, dtor_end, {context.clazz_ptr_type} };
        clazz_type->setBody(in_context.field_vec, false); // update StructType for GEP

        // clazz and ctor/dtor fields
        for(auto& field_var_it : var.field_var_list)
        {
            auto field_var = field_var_it.value();
            execute(field_var, in_context);
        }

        {
            // begin ctor body
            auto clazz_size = the_module->getDataLayout().getTypeAllocSize(clazz_type);
            auto clazz_size_const = llvm::ConstantInt::get(llvm::Type::getInt64Ty(the_llvm), clazz_size);
            auto malloc_call = llvm::CallInst::Create(the_malloc_func, clazz_size_const, nil, ctor_start);
            auto malloc_cast = new llvm::BitCastInst(malloc_call, clazz_ptr_type, nil, ctor_start);
            new llvm::StoreInst(malloc_cast, ctor_alloca, false, ctor_start);

            // store context clazz in the first field
            llvm::Function::arg_iterator arg_it = ctor_func->arg_begin();
            llvm::Value* arg = &*arg_it;
            arg->setName("arg");
            auto context_field = get_clazz_field(clazz_type, malloc_cast, 0, ctor_start);
            new llvm::StoreInst(arg, context_field, false, ctor_start);
            ctor_start->removeFromParent();

            // end ctor body
            llvm::BranchInst::Create(ctor_end, in_context.ctor_entry);
            auto clazz_load = new llvm::LoadInst(ctor_alloca, nil, false, ctor_end);
            llvm::ReturnInst::Create(the_llvm, clazz_load, ctor_end);
        }

        {
            // begin dtor body
            llvm::Function::arg_iterator arg_it = dtor_func->arg_begin();
            llvm::Value* arg = &*arg_it;
            arg->setName("arg");
            new llvm::StoreInst(arg, dtor_alloca, false, dtor_start);
            dtor_start->removeFromParent();

            // end dtor body
            llvm::BranchInst::Create(in_context.dtor_entry, dtor_begin);
            auto clazz_load = new llvm::LoadInst(dtor_alloca, nil, false, dtor_end);
            auto int8_ptr_type = llvm::PointerType::get(llvm::Type::getInt8Ty(the_llvm), 0);
            auto free_cast = new llvm::BitCastInst(clazz_load, int8_ptr_type, nil, dtor_end);
            llvm::CallInst::Create(the_free_func, free_cast, nil, dtor_end);
            llvm::ReturnInst::Create(the_llvm, dtor_end);
        }
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
        auto clazz_load = new llvm::LoadInst(context.ctor_alloca, nil, false, context.ctor_entry);

        core::Shared<Context> share = context;
        while(share->var != loc.context_var)
        {
            auto field = get_clazz_field(clazz_load, 0, context.ctor_entry);
            clazz_load = new llvm::LoadInst(field, nil, false, context.ctor_entry);
            share = share->outer;
        }

        auto field_pos = loc.field_pos + (share->outer != core::nil);
        auto field = get_clazz_field(clazz_load, field_pos, context.ctor_entry);
        return new llvm::LoadInst(field, nil, false, context.ctor_entry);
    }
    llvm::Value* execute(FilterLocation& loc, Context& context)
    {
        core::certify(false);
        return 0;
    }
    llvm::Value* execute(NestIdLocation& loc, Context& context)
    {
        auto loc_val = execute(loc.loc, context);
        auto field_pos = loc.field_pos + 1;
        auto field = get_clazz_field(loc_val, field_pos, context.ctor_entry);
        return new llvm::LoadInst(field, nil, false, context.ctor_entry);
    }
    llvm::Value* execute(NestFilterLocation& loc, Context& context)
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
                val1 = new llvm::SIToFPInst(val1, type2, nil, context.ctor_entry);
            else if(type2->isIntegerTy())
                val2 = new llvm::SIToFPInst(val2, type1, nil, context.ctor_entry);
            else if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::FPExtInst(val2, type1, nil, context.ctor_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::FPExtInst(val1, type2, nil, context.ctor_entry);
            return execute_float(val1, op, val2, context);
        }
        else if(type1->isIntegerTy() && type2->isIntegerTy())
        {
            if(type1->getPrimitiveSizeInBits() > type2->getPrimitiveSizeInBits())
                val2 = new llvm::SExtInst(val2, type1, nil, context.ctor_entry);
            else if(type2->getPrimitiveSizeInBits() > type1->getPrimitiveSizeInBits())
                val1 = new llvm::SExtInst(val1, type2, nil, context.ctor_entry);
            return execute_int(val1, op, val2, context);
        }
        else
        {
            throw env::Format("Cannot handle llvm::Type::TypeID's: [%1, %2]")
                % type1->getTypeID()
                % type2->getTypeID()
                % env::exception;
        }
    }
    llvm::Value* execute_int(llvm::Value* val1, BinaryOp op, llvm::Value* val2, Context& context)
    {
        switch(op)
        {
            case BinaryOp::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Mul, val1, val2, nil, context.ctor_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, val1, val2, nil, context.ctor_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::Add, val1, val2, nil, context.ctor_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, val1, val2, nil, context.ctor_entry);
            case BinaryOp::SHL:
                return llvm::BinaryOperator::Create(llvm::Instruction::Shl, val1, val2, nil, context.ctor_entry);
            case BinaryOp::SHR:
                return llvm::BinaryOperator::Create(llvm::Instruction::AShr, val1, val2, nil, context.ctor_entry);
            case BinaryOp::EQ:
                return new llvm::ICmpInst(*context.ctor_entry, llvm::ICmpInst::ICMP_EQ, val1, val2, nil);
            case BinaryOp::NE:
                return new llvm::ICmpInst(*context.ctor_entry, llvm::ICmpInst::ICMP_NE, val1, val2, nil);
            case BinaryOp::LT:
                return new llvm::ICmpInst(*context.ctor_entry, llvm::ICmpInst::ICMP_SLT, val1, val2, nil);
            case BinaryOp::GT:
                return new llvm::ICmpInst(*context.ctor_entry, llvm::ICmpInst::ICMP_SGT, val1, val2, nil);
            case BinaryOp::LE:
                return new llvm::ICmpInst(*context.ctor_entry, llvm::ICmpInst::ICMP_SLE, val1, val2, nil);
            case BinaryOp::GE:
                return new llvm::ICmpInst(*context.ctor_entry, llvm::ICmpInst::ICMP_SGE, val1, val2, nil);
            case BinaryOp::AND:
                return llvm::BinaryOperator::Create(llvm::Instruction::And, val1, val2, nil, context.ctor_entry);
            case BinaryOp::OR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, val1, val2, nil, context.ctor_entry);
            case BinaryOp::XOR:
                return llvm::BinaryOperator::Create(llvm::Instruction::Xor, val1, val2, nil, context.ctor_entry);
            case BinaryOp::MOD:
                return llvm::BinaryOperator::Create(llvm::Instruction::SRem, val1, val2, nil, context.ctor_entry);
        }
        return 0;
    }
    llvm::Value* execute_float(llvm::Value* val1, BinaryOp op, llvm::Value* val2, Context& context)
    {
        switch(op)
        {
            case BinaryOp::MUL:
                return llvm::BinaryOperator::Create(llvm::Instruction::FMul, val1, val2, nil, context.ctor_entry);
            case BinaryOp::DIV:
                return llvm::BinaryOperator::Create(llvm::Instruction::FDiv, val1, val2, nil, context.ctor_entry);
            case BinaryOp::ADD:
                return llvm::BinaryOperator::Create(llvm::Instruction::FAdd, val1, val2, nil, context.ctor_entry);
            case BinaryOp::SUB:
                return llvm::BinaryOperator::Create(llvm::Instruction::FSub, val1, val2, nil, context.ctor_entry);
            case BinaryOp::EQ:
                return new llvm::FCmpInst(*context.ctor_entry, llvm::FCmpInst::FCMP_OEQ, val1, val2, nil);
            case BinaryOp::NE:
                return new llvm::FCmpInst(*context.ctor_entry, llvm::FCmpInst::FCMP_UNE, val1, val2, nil);
            case BinaryOp::LT:
                return new llvm::FCmpInst(*context.ctor_entry, llvm::FCmpInst::FCMP_OLT, val1, val2, nil);
            case BinaryOp::GT:
                return new llvm::FCmpInst(*context.ctor_entry, llvm::FCmpInst::FCMP_OGT, val1, val2, nil);
            case BinaryOp::LE:
                return new llvm::FCmpInst(*context.ctor_entry, llvm::FCmpInst::FCMP_OLE, val1, val2, nil);
            case BinaryOp::GE:
                return new llvm::FCmpInst(*context.ctor_entry, llvm::FCmpInst::FCMP_OGE, val1, val2, nil);
            case BinaryOp::SHL:
            case BinaryOp::SHR:
            case BinaryOp::AND:
            case BinaryOp::OR:
            case BinaryOp::XOR:
            case BinaryOp::MOD:
                throw env::Format("Binary operator '%1' not supported for floating point")
                    % binary_op_name(op)
                    % env::exception;
        }
    }
    llvm::Value* get_clazz_field(llvm::Value* value, int position, llvm::BasicBlock* entry)
    {
        auto type = value->getType()->getContainedType(0);
        auto int32_0_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        auto int32_p_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), position);
        return llvm::GetElementPtrInst::Create(type, value, {int32_0_const, int32_p_const}, nil, entry);
    }
    llvm::Value* get_clazz_field(llvm::Type* type, llvm::Value* value, int position, llvm::Instruction* before)
    {
        auto int32_0_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), 0);
        auto int32_p_const = llvm::ConstantInt::get(llvm::Type::getInt32Ty(the_llvm), position);
        return llvm::GetElementPtrInst::Create(type, value, {int32_0_const, int32_p_const}, nil, before);
    }
    template<class Type>
    env::Exception bad_class_exception(Type& var)
    {
        return env::Format("Writable: %1 not handled")
            % typeid(var).name()
            % env::exception;
    }
private:
    llvm::Function* the_malloc_func;
    llvm::Function* the_free_func;
    llvm::LLVMContext the_llvm;
    std::unique_ptr<llvm::Module> the_module;
    const char* nil = "";
};
