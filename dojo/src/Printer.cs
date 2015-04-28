
using System;

class Printer
{
	Iterator it;
	Iterator proto;
	HTML html;
	JS js;

	public Printer(ast.Define root)
	{
		it = new Iterator(new Solver(root));
		html = new HTML();
		js = new JS();
	}
	public void Write(string name, string output)
	{
		try
		{
			html.Open(output + ".html");
			js.Open(output + ".js");
			html.Put(html.body);
			html.Put(name);
			js.Put(js.header);
			WriteBody(it);
			WriteLayout(it);
			html.Close();
			js.Close();
		}
		catch(Exception exception)
		{
			html.Delete();
			js.Delete();
			Console.Out.WriteLine(exception.Message + "\nStack Trace:\n" + exception.StackTrace);
		}
		finally
		{
			html.Dispose();
			js.Dispose();
		}
	}
	void WriteDefine(Iterator it)
	{
		if(it.IsProto)
		{
			if(proto == null)
				proto = it;
			var format = js.Put(js.proto);
			js.Put(it.Object.identity.name);
			var comma = "";
			foreach(var param in it.Object.paramList)
			{
				js.Write(comma);
				js.Write(param.identity.name);
				comma = js.comma;
			}
			js.Put();
			if(it.IsComplex)
			{
				js.Enter();
				js.Put(js.protoClazz);
				WriteBody(it);
				WriteLayout(it);
				js.Leave();
			}
			if(proto == it)
				proto = null;
			js.End(format);
		}
		else if(it.IsComplex)
		{
			var format = js.Put(js.clazz);
			js.Put(it.Object.identity.name);
			WriteBody(it);
			WriteLayout(it);
			js.End(format);
		}
		else if(proto != null)
		{
			var format = js.Put(js.keyClone);
			js.Put(it.Object.identity.name);
			js.Put(it.Variable, format);
		}
		else
		{
			var format = js.Put(js.keyValue);
			js.Put(it.Object.identity.name);
			js.Put(it.Value, format);
		}
	}
	void WriteBody(Iterator it)
	{
		js.Enter();
		while(it.Next)
		{
			var child = it.Enter();
			WriteDefine(child);
			it.Leave();
		}
		js.Leave();
	}
	void WriteLayout(Iterator it)
	{
		var format = js.Dojo(it.Object.ToString());
		if(format != null)
		{
			js.Enter();
			var key = js.Put(format);
			js.Put(it.Object.identity.name);
			js.Put(it.Object.identity.name, key);
			while(it.Next)
			{
				var ch = it.Enter();
				format = js.DojoChild(it.Object.ToString());
				if(format != null)
				{
					var child = js.Put(format);
					js.Put(it.Object.identity.name);
					js.Put(ch.Object.identity.name, child);
				}
				it.Leave();
			}
			js.Leave();
		}
	}
}
