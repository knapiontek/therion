
using System;
using System.Diagnostics;
using System.Collections.Generic;
using Mono.Cecil;
using Mono.Cecil.Cil;

class Builder
{
	coco.Parser parser;
	il.Assembly assembly;

	public Builder(coco.Parser parser)
	{
		this.parser = parser;
	}
	public void WriteAssembly(string name, string output, ast.Line root)
	{
		assembly = il.Assembly.Create(name);
		CreateMethodBody(assembly.MainMethod, root);
		assembly.Write(output);
	}
	void CreateMethodBody(MethodDefinition method, ast.Line root)
	{
		foreach(var line in root.childList)
		{
			switch(line.kind)
			{
			case ast.Line.Kind.CODE:
				CreateCode(method, line);
				break;
			case ast.Line.Kind.CLASS:
				CreateClass(method, line);
				break;
			case ast.Line.Kind.FUNCTION:
				CreateFunction(method, line);
				break;
			default:
				Debug.Assert(false);
				break;
			}
		}
		assembly.Return(method);
	}
	void CreateCode(MethodDefinition parent, ast.Line line)
	{
		for(int i = line.expr.itemList.Count - 2; i >= 0; i--)
		{
			var itemL = line.expr.itemList[i];
			var itemR = line.expr.itemList[i + 1];
			if(itemL is ast.Const)
			{
				parser.SemErr("Constant cannot evaluate");
			}
			else if(itemL is ast.Call)
			{
				var callL = itemL as ast.Call;
				var nameL = callL.path.partList[0].name;
				if(itemR is ast.Const)
				{
					var cnst = itemR as ast.Const;
					var type = assembly.ConstType(cnst);
					if(assembly.IsCtor(parent))
					{
						var field = assembly.CreateField(nameL, type, parent);
						assembly.LoadConst(cnst, parent);
						assembly.StoreField(field, parent);
					}
					else
					{
						var variable = assembly.CreateVariable(nameL, type, parent);
						assembly.LoadConst(cnst, parent);
						assembly.StoreVariable(variable, parent);
					}
				}
				else if(itemR is ast.Call)
				{
					var callR = itemR as ast.Call;
					var type = assembly.BasicType(callR.path.partList[0].name);
				}
			}
		}
	}
	void CreateClass(MethodDefinition parent, ast.Line root)
	{
		var call = root.expr.itemList[0] as ast.Call;
		foreach(var part in call.path.partList)
		{
			var name = part.name;
			var type = assembly.CreateType(name, parent);
			var ctor = assembly.CreateCtor(name, type);
			assembly.CreateField(name, type, parent);
			CreateMethodBody(ctor, root);
			parent = ctor;
		}
	}
	void CreateFunction(MethodDefinition parent, ast.Line root)
	{
		var call = root.expr.itemList[0] as ast.Call;
		foreach(var part in call.path.partList)
		{
			var name = part.name;
			var method = assembly.CreateMethod(name, parent);
			CreateMethodBody(method, root);
			parent = method;
		}
	}
}
