
using System;
using System.Collections.Generic;

class Printer
{
	CLang clang = new CLang();

	public void Write(ast.Define root, string output)
	{
		try
		{
			clang.Open(output + ".c");
			clang.Put(clang.finalDef);
			WriteTypeFamily(root);
			clang.Begin(clang.fieldDef)
				.Put(root.Type.Name)
				.Put(root.Name);
			WriteCopyFamily(root);
			WriteCallFamily(root);
			clang.Begin(clang.mainDef)
				.Put(root.Type.Name);
			clang.Close();
		}
		catch(Exception exception)
		{
			clang.Delete();
			Console.Out.WriteLine(exception.Message + "\nStack Trace:\n" + exception.StackTrace);
		}
		finally
		{
			clang.Dispose();
		}
	}
	void WriteTypeFamily(ast.Define define)
	{
		foreach(var field in define.fieldList)
			WriteTypeFamily(field);
		foreach(var clone in define.cloneList)
			WriteTypeFamily(clone);
		if(define.IsType && define.IsReferred && define.IsPrintable)
			WriteType(define);
	}
	void WriteType(ast.Define define)
	{
		var format = clang.Begin(clang.typeDef);
		format.Put(define.Name);
		clang.Enter();
		if(define.NeedThat)
			clang.Begin(clang.thatDef)
				.Put(define.that.Type.Name)
				.Put(define.that.Name);
		if(define.link.Tail != ast.Define.nil)
			clang.Begin(clang.fieldDef)
				.Put(define.link.Tail.Type.Name)
				.Put(define.link.Tail.Name);
		foreach(var field in define.fieldList)
			if(!field.IsCall && field.IsReferred && field.IsPrintable)
				clang.Begin(clang.fieldDef)
					.Put(field.Type.Name)
					.Put(field.Name);
		foreach(var clone in define.cloneList)
			if(!clone.IsCall && clone.IsReferred && clone.IsPrintable)
				clang.Begin(clang.fieldDef)
					.Put(clone.Type.Name)
					.Put(clone.Name);
		clang.Leave();
		format.Put();
		format.Put(define.Name);
		format.Put(define.Type.Name);
	}
	void WriteCopyFamily(ast.Define define)
	{
		foreach(var field in define.fieldList)
			WriteCopyFamily(field);
		foreach(var clone in define.cloneList)
			WriteCopyFamily(clone);
		if(define.IsType && define.NeedCopy)
			WriteCopy(define);
	}
	void WriteCopy(ast.Define define)
	{
		var format = clang.Begin(clang.copyDef);
		format.Put(define.Type.Name);
		format.Put(define.Type.Name);
		format.Put(define.Type.Name);
		clang.Enter();
		if(define.NeedThat)
			clang.Begin(clang.finalCopy)
				.Put(define.that.Name)
				.Put(define.that.Name);
		if(define.link.Tail != ast.Define.nil)
			WriteCopyField(define.link.Tail);
		foreach(var field in define.fieldList)
			if(!field.IsCall && field.IsReferred && field.IsPrintable)
				WriteCopyField(field);
		foreach(var clone in define.cloneList)
			if(!clone.IsCall && clone.IsReferred && clone.IsPrintable)
				WriteCopyField(clone);
		clang.Leave();
		format.End();
	}
	void WriteCopyField(ast.Define define)
	{
		if(define.IsFinal)
			clang.Begin(clang.finalCopy)
				.Put(define.Name)
				.Put(define.Name);
		else
			clang.Begin(clang.typeCopy)
				.Put(define.Type.Name)
				.Put(define.Name)
				.Put(define.Name);
	}
	void WriteCallFamily(ast.Define define)
	{
		foreach(var field in define.fieldList)
			if(field.IsPrintable)
				WriteCallFamily(field);
		foreach(var clone in define.cloneList)
			if(clone.IsPrintable)
				WriteCallFamily(clone);
		if((define.IsCall || define.IsType) && define.IsReferred && define.IsPrintable)
			WriteCall(define);
	}
	void WriteCall(ast.Define define)
	{
		var format = clang.Begin(clang.callDef);
		format.Put(define.Call.Name);
		if(!define.IsRoot)
			clang.Begin(clang.firstParamDef)
				.Put(define.Type.Name)
				.Put(define.Name);
		format.Put();
		if(define.NeedThat)
			clang.Begin(clang.typeNextParamDef)
				.Put(define.that.Type.Name)
				.Put(define.that.Name);
		foreach(var param in define.paramList)
			if(param.IsFinal)
				clang.Begin(clang.finalNextParamDef)
					.Put(param.Type.Name)
					.Put(param.Name);
			else
				clang.Begin(clang.typeNextParamDef)
					.Put(param.Type.Name)
					.Put(param.Name);
		format.Put();
		clang.Enter();
		if(define.IsType && define.NeedThat)
			clang.Begin(clang.thatCall)
				.Put(define.Name)
				.Put(define.that.Name)
				.Put(define.that.Name);
		if(define.link.Tail != ast.Define.nil)
			WriteCallField(define.link.Tail, define.link, define);
		foreach(var field in define.fieldList)
			WriteCallField(field, define);
		foreach(var clone in define.cloneList)
			WriteCallField(clone, define);
		clang.Leave();
		format.End();
	}
	void WriteCallField(ast.Define field, ast.Define define)
	{
		if(field.IsSnapped)
			clang.Begin(clang.label).Put(field.Name);
		if(!field.IsCall && field.IsPrintable)
		{
			if(field.IsType && !field.IsSpecial)
			{
				if(field.NeedThat)
					clang.Begin(clang.typeWithThatCall)
						.Put(field.Name)
						.Put(NameType(field, define))
						.Put(define.Name);
				else
					clang.Begin(clang.typeCall)
						.Put(field.Name)
						.Put(NameType(field, define));
			}
			else
			{
				WriteCallField(field, field.link, define);
				if(field.IsCase)
				{
					var format = clang.Begin(clang.caseBlock);
					format.Put(field.Name);
					clang.Enter();
					foreach(var local in field.fieldList)
						WriteCallField(local, define);
					foreach(var clone in field.cloneList)
						WriteCallField(clone, define);
					clang.Leave();
					format.End();
				}
			}
		}
	}
	class Route
	{
		string value;
		int level;
		bool final;

		public Route(ast.Define that, ast.Define from)
		{
			var it = from != that
				? from.that
				: from;
			value = it.Name;
			while(it != that)
			{
				it = it.that;
				value += "->" + it.Name;
			}
			level = 1;
			final = false;
		}
		public Route(string value)
		{
			this.value = value;
			level = 0;
			final = true;
		}
		public Route(string value, ast.Part part)
		{
			this.value = value;
			level = part.define.IsParam ? 1 : 0;
			final = part.define.IsFinal;
		}
		public void Append(ast.Part part)
		{
			var access = this.level == 0 ? "." : "->";
			value += access + part.define.Name;
			level = part.define.IsParam ? 1 : 0;
			final = part.define.IsFinal;
		}
		public string Value
		{
			get { return level == 0 || final ? value : "*" + value; }
		}
		public string Address
		{
			get { return level == 0 ? "&" + value : value; }
		}
		public string Param
		{
			get { return level == 0 && !final ? "&" + value : value; }
		}
	}
	Route WriteCallField(ast.Define define, ast.Link link, ast.Define from)
	{
		Route route = null;
		if(link is ast.Final)
		{
			route = new Route(link.ToString());
			if(define != null)
				WriteCallTail(define, from, route);
		}
		else if(link is ast.Path)
		{
			var path = link as ast.Path;
			for(var i = 0; i < path.partList.Count; i++)
			{
				var part = path.partList[i];
				var tail = define != null && i == path.partList.Count - 1;
				if(i == 0)
				{
					if(part.define.IsParam || part.define.that.IsRoot)
						route = new Route(part.define.that.Name, part);
					else
						route = new Route(part.define.that, from);
				}
				if(part.paramList != ast.LinkList.none)
				{
					List<Route> paramList = new List<Route>();
					if(part.define.NeedThat)
						paramList.Add(route);
					foreach(var param in part.paramList)
						paramList.Add(WriteCallField(null, param, from));
					if(tail)
						WriteCallTail(define, from, part, paramList);
					else
					{
						WriteCallLocal(part, paramList);
						route = new Route(part.Name, part);
					}
				}
				else
				{
					if(part.define.IsParam || !part.define.IsReferred)
						route = new Route(part.define.Name, part);
					else
						route.Append(part);
					if(tail)
						WriteCallTail(define, from, route);
				}
			}
		}
		return route;
	}
	void WriteCallLocal(ast.Part part, List<Route> paramList)
	{
		if(part.define.IsBuiltIn)
		{
			clang.Begin(clang.opLocalCall)
				.Put(part.define.Type.Name)
				.Put(part.Name)
				.Put(paramList[0].Value)
				.Put(part.define.identity.val)
				.Put(paramList[1].Value);
		}
		else
		{
			var format = clang.Begin(clang.typeLocalCall);
			format.Put(part.define.Type.Name);
			format.Put(part.Name);
			format.Put(part.define.Call.Name);
			format.Put(part.Name);
			foreach(var param in paramList)
				clang.Begin(clang.paramCall).Put(param.Param);
			format.End();
		}
	}
	void WriteCallTail(ast.Define define, ast.Define from, Route route)
	{
		if(define.IsReferred)
		{
			if(define.IsFinal)
				clang.Begin(clang.finalTailCall)
					.Put(NameFinal(define, from))
					.Put(route.Value);
			else
				clang.Begin(clang.typeCopyCall)
					.Put(define.Type.Name)
					.Put(NameType(define, from))
					.Put(route.Address);
		}
		else
		{
			if(define.IsFinal)
				clang.Begin(clang.finalLocalCall)
					.Put(define.Type.Name)
					.Put(define.Name)
					.Put(route.Value);
			else
				clang.Begin(clang.typeLocalCopyCall)
					.Put(define.Type.Name)
					.Put(define.Name)
					.Put(define.Type.Name)
					.Put(define.Name)
					.Put(route.Address);
		}
	}
	void WriteCallTail(ast.Define define, ast.Define from, ast.Part part, List<Route> paramList)
	{
		if(define.IsReferred)
		{
			if(part.define.IsBuiltIn)
			{
				clang.Begin(clang.opTailCall)
					.Put(NameFinal(define, from))
					.Put(paramList[0].Value)
					.Put(part.define.identity.val)
					.Put(paramList[1].Value);
			}
			else
			{
				var format = clang.Begin(clang.typeTailCall);
				format.Put(define.Call.Name);
				format.Put(NameType(define, from));
				foreach(var param in paramList)
					clang.Begin(clang.paramCall).Put(param.Param);
				format.End();
			}
		}
		else
		{
			if(part.define.IsBuiltIn)
			{
				clang.Begin(clang.opLocalCall)
					.Put(define.Type.Name)
					.Put(define.Name)
					.Put(paramList[0].Value)
					.Put(part.define.identity.val)
					.Put(paramList[1].Value);
			}
			else
			{
				var format = clang.Begin(clang.typeLocalCall);
				format.Put(define.Type.Name);
				format.Put(define.Name);
				format.Put(define.Call.Name);
				format.Put(define.Name);
				foreach(var param in paramList)
					clang.Begin(clang.paramCall).Put(param.Param);
				format.End();
			}
		}
	}
	string NameType(ast.Define define, ast.Define from)
	{
		if(!from.IsType)
			return from.Name;
		else if(from.IsRoot)
			return "&" + from.Name + "." + define.Name;
		else
			return "&" + from.Name + "->" + define.Name;
	}
	string NameFinal(ast.Define define, ast.Define from)
	{
		if(!from.IsType)
			return "*" + from.Name;
		else if(from.IsRoot)
			return from.Name + "." + define.Name;
		else
			return from.Name + "->" + define.Name;
	}
}
