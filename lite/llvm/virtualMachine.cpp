
class VirtualMachine
{
public:
    PointerType* pointer_type(llvm::Type* type) { return = PointerType::get(type, 0); }

    IntegerType* int8_type() { return IntegerType::get(the_context, 8); }
    IntegerType* int16_type() { return IntegerType::get(the_context, 16); }
    IntegerType* int32_type() { return IntegerType::get(the_context, 32); }
    IntegerType* int64_type() { return IntegerType::get(the_context, 64); }
    FloatType* float32_type() { return FloatType::get(the_context, 0); }
    DoubleType* float64_type() { return DoubleType::get(the_context, 0); }

    ConstantInt* int8_const(core::int8 value) { return ConstantInt::get(int8_type(), value); }
    ConstantInt* int16_const(core::int16 value) { return ConstantInt::get(int16_type(), value); }
    ConstantInt* int32_const(core::int32 value) { return ConstantInt::get(int32_type(), value); }
    ConstantInt* int64_const(core::int64 value) { return ConstantInt::get(int64_type(), value); }
    ConstantFP* float32_const(core::float32 value) { return ConstantFP::get(float32_type(), value); }
    ConstantFP* float64_const(core::float64 value) { return ConstantFP::get(float64_type(), value); }

    Function* function(core::String& name, Type* result, core::Seq<Type*>& arg_seq)
    {
        core::verify(!the_module->getFunction(name.ascii()))
        ArrayRef<Type*> array_ref(arg_seq.data(), arg_seq.size());
        FunctionType* type = FunctionType::get(result, array_ref, false);
        return Function::Create(type, GlobalValue::ExternalLinkage, name.ascii(), the_module);
    }
    CallInst* call(Function* function, Value* value)
    {
        return CallInst::Create(the_context, value, "call", block);
    }
    BasicBlock* block(core::String& name, Function* function)
    {
        return BasicBlock::Create(the_context, name.ascii(), function, 0);
    }
    AllocaInst* alloca(core::String& name, Type* type, BasicBlock* block)
    {
        return new AllocaInst(type, name.acii(), block);
    }
    StoreInst* store(Value* value, AllocaInst* alloca)
    {
        return new StoreInst(value, alloca, false, block);
    }
    LoadInst* load(AllocaInst* alloca)
    {
        return new LoadInst(alloca, "alloca", false, block);
    }
    GetElementPtrInst* element(LoadInst* load_inst, StructType* type, int64 index)
    {
        return GetElementPtrInst::Create(load_inst->GetType(), load_inst, { int64_const(0), int64_const(index) }, block);
    }
    CastInst* truncate(Value* value, Type* type)
    {
        return new TruncInst(value, type, "trancate", block);
    }
    ReturnInst* return_(Value* value)
    {
        return ReturnInst::Create(the_context, value, block);
    }
private:
    llvm::Module* the_module;
};
