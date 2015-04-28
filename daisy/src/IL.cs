
using System;
using System.Diagnostics;
using System.Collections.Generic;
using Mono.Cecil;
using Mono.Cecil.Cil;

namespace il
{
	public class Assembly
	{
		internal static string CTOR = "ctor";
		internal TypeReference[] constTypes = null;
		internal Dictionary<string, TypeReference> basicTypes = null;

		internal TypeDefinition mainType;
		internal MethodDefinition mainMethod;
		internal AssemblyDefinition assembly;

		public static Assembly Create(string name)
		{
			return new Assembly(name);
		}
		internal Assembly(string name)
		{
			var def = new AssemblyNameDefinition(name, new Version (0, 0, 0, 0));
			assembly = AssemblyDefinition.CreateAssembly(def, name, ModuleKind.Console);
			mainType = new TypeDefinition(null, name, TypeAttributes.NotPublic, assembly.MainModule.TypeSystem.Object);
			assembly.MainModule.Types.Add(mainType);
			mainMethod = CreateFunction(name, mainType);
			assembly.MainModule.EntryPoint = mainMethod;
		}
		public void Write(string output)
		{
			assembly.Write(output);
		}
		public TypeDefinition CreateType(string name, MethodDefinition parent)
		{
			var fullName = parent.DeclaringType.Name + "." + name;
			var attributes = TypeAttributes.Public;
			var valueType = assembly.MainModule.Import(typeof(System.ValueType));
			var type = new TypeDefinition(null, fullName, attributes, valueType);
			assembly.MainModule.Types.Add(type);
			return type;
		}
		public MethodDefinition CreateCtor(string name, TypeDefinition type)
		{
			var attributes = MethodAttributes.Public;
			var method = new MethodDefinition(CTOR, attributes, assembly.MainModule.TypeSystem.Void);
			type.Methods.Add(method);
			return method;
		}
		public MethodDefinition CreateMethod(string name, MethodDefinition parent)
		{
			var attributes = MethodAttributes.Public;
			var method = new MethodDefinition(name, attributes, assembly.MainModule.TypeSystem.Void);
			parent.DeclaringType.Methods.Add(method);
			return method;
		}
		public MethodDefinition CreateFunction(string name, TypeDefinition parent)
		{
			var attributes = MethodAttributes.Public | MethodAttributes.Static;
			var method = new MethodDefinition(name, attributes, assembly.MainModule.TypeSystem.Void);
			parent.Methods.Add(method);
			return method;
		}
		public FieldDefinition CreateField(string name, TypeReference type, MethodDefinition parent)
		{
			var attributes = FieldAttributes.Public;
			var field = new FieldDefinition(name, attributes, type);
			parent.DeclaringType.Fields.Add(field);
			return field;
		}
		public VariableDefinition CreateVariable(string name, TypeDefinition type, MethodDefinition parent)
		{
			var variable = new VariableDefinition(name, type);
			parent.Body.Variables.Add(variable);
			return variable;
		}
		public TypeReference ConstType(ast.Const cnst)
		{
			if(constTypes == null)
			{
				constTypes = new TypeReference[]
				{
					assembly.MainModule.TypeSystem.String,
	
					assembly.MainModule.TypeSystem.SByte,
					assembly.MainModule.TypeSystem.Int16,
					assembly.MainModule.TypeSystem.Int32,
					assembly.MainModule.TypeSystem.Int64,
	
					assembly.MainModule.TypeSystem.Byte,
					assembly.MainModule.TypeSystem.UInt16,
					assembly.MainModule.TypeSystem.UInt32,
					assembly.MainModule.TypeSystem.UInt64,
	
					assembly.MainModule.TypeSystem.Single,
					assembly.MainModule.TypeSystem.Double
				};
			}
			return constTypes[(int)cnst.kind];
		}
		public TypeReference BasicType(string name)
		{
			if(basicTypes == null)
			{
				basicTypes = new Dictionary<string, TypeReference>()
				{
					{ "byte", assembly.MainModule.TypeSystem.Byte },
					{ "int", assembly.MainModule.TypeSystem.Int32 },
					{ "uint", assembly.MainModule.TypeSystem.UInt32 },
					{ "float", assembly.MainModule.TypeSystem.Double },
	
					{ "string", assembly.MainModule.TypeSystem.String },
	
					{ "int1", assembly.MainModule.TypeSystem.SByte },
					{ "int2", assembly.MainModule.TypeSystem.Int16 },
					{ "int4", assembly.MainModule.TypeSystem.Int32 },
					{ "int8", assembly.MainModule.TypeSystem.Int64 },
	
					{ "uint1", assembly.MainModule.TypeSystem.Byte },
					{ "uint2", assembly.MainModule.TypeSystem.UInt16 },
					{ "uint4", assembly.MainModule.TypeSystem.UInt32 },
					{ "uint8", assembly.MainModule.TypeSystem.UInt64 },
	
					{ "float4", assembly.MainModule.TypeSystem.Single },
					{ "float8", assembly.MainModule.TypeSystem.Double }
				};
			}
			TypeReference type = null;
			basicTypes.TryGetValue(name, out type);
			return type;
		}
		public TypeDefinition MainType
		{
			get { return mainType; }
		}
		public MethodDefinition MainMethod
		{
			get { return mainMethod; }
		}
		public bool IsCtor(MethodDefinition method)
		{
			return method.Name == CTOR;
		}
		public void LoadConst(ast.Const cnst, MethodDefinition method)
		{
			method.Body.GetILProcessor().Emit(OpCodes.Ret);
		}
		public void StoreField(FieldDefinition field, MethodDefinition method)
		{
			method.Body.GetILProcessor().Emit(OpCodes.Ret);
		}
		public void Return(MethodDefinition method)
		{
			method.Body.GetILProcessor().Emit(OpCodes.Ret);
		}
	}
}
