
using System;
using System.Diagnostics;
using System.Collections.Generic;

class Builder
{
	coco.Parser parser;
	ast.NameDict nameDict;
	HTML html;
	JavaScript js;
	int tableCounter = 0;
	int trCounter = 0;
	
	public Builder(coco.Parser parser)
	{
		this.parser = parser;
		html = new HTML();
		js = new JavaScript();
	}
	public void Write(string name, string output, ast.Line root, ast.NameDict nameDict)
	{
		this.nameDict = nameDict;
		try
		{
			html.Open(output + ".html");
			js.Open(output + ".js");

			html.Put(html.body);
			html.Put(name);

			js.Write(js.require);
		
			foreach(var line in root.childList)
			{
				CreateBody(line);
			}

			html.Close();
			js.Close();
		}
		catch(Exception exception)
		{
			html.Delete();
			js.Delete();
			parser.SemErr(exception.Message + "\nStack Trace:\n" + exception.StackTrace);
		}
		finally
		{
			html.Dispose();
			js.Dispose();
		}
	}
	void CreateBody(ast.Line line)
	{
		var node = line.define.node;
		if(node == null)
		{
			if(line.childList.Count > 0)
				CreateClazzBody(line);
		}
		else if(node is ast.Final)
		{
			var final = node as ast.Final;
			if(final.kind == ast.Final.Kind.UNICODE)
			{
				var keyUnicode = js.Put(js.keyUnicode);
				js.Put(line.name);
				js.Put(final.value, keyUnicode);
			}
			else
			{
				var keyValue = js.Put(js.keyValue);
				js.Put(line.name);
				js.Put(final.value, keyValue);
			}
		}
		else if(node is ast.Call)
		{
			var call = node as ast.Call;
			if(line.define.identity.name.Equals("main_layout") && call.path.name.Equals("layout"))
			{
				CreateNodeChildren(line);
			}
		}
	}
	void CreateClazz(ast.Line root)
	{
		var clazz = js.Put(js.clazz);
		js.Put(root.define.identity.name);
		string comma = string.Empty;
		foreach(var param in root.define.paramList)
		{
			js.Write(comma);
			comma = js.comma;
			js.Write(param.identity.name);
		}
		js.Put(string.Empty);
		CreateClazzBody(root);
		js.End(clazz);
	}
	void CreateClazzBody(ast.Line root)
	{
		foreach(var line in root.childList)
		{
			var name = line.define.identity.name;
			var node = line.define.node;
			if(line.childList != ast.LineList.empty)
			{
				CreateClazz(line);
			}
			if(node == null)
			{

			}
			else if(node is ast.Final)
			{
				var final = node as ast.Final;
				var value = final.value;
				if(final.kind == ast.Final.Kind.UNICODE)
				{
					var keyUnicode = js.Put(js.keyUnicode);
					js.Put(name);
					js.Put(value, keyUnicode);
				}
				else
				{
					var keyValue = js.Put(js.keyValue);
					js.Put(name);
					js.Put(value, keyValue);
				}
			}
			else if(node is ast.Call)
			{
				var call = node as ast.Call;
				var value = call.path.name;
				if(ast.Keywords.isKeyword(value))
				{
					var keyValue = js.Put(js.keyValue);
					js.Put(name);
					js.Put(value, keyValue);
				}
				else if(root.define.Param(value) != null)
				{
					var keyClone = js.Put(js.keyClone);
					js.Put(name);
					js.Put(value, keyClone);
				}
				else if(nameDict.ContainsKey(value))
				{
					var keyClone = js.Put(js.keyClone);
					js.Put(name);
					js.Put(value, keyClone);
				}
				else
				{
					parser.SemErr("Variable: '" + value + "' does NOT exist");
				}
			}
		}
	}
	void CreateNodeChildren(ast.Line root)
	{
		foreach(var line in root.childList)
		{
			if(trCounter > 0)
			{
				var td = html.Put(html.td);
				CreateNode(line);
				html.End(td);
			}
			else
			{
				CreateNode(line);
			}
		}
	}
	void CreateNode(ast.Line line)
	{
		var name = line.define.identity.name;
		var node = line.define.node;
		if(node == null)
		{
			if(name.StartsWith("y_pane"))
			{
				var table = html.Put(html.table);
				tableCounter++;
				CreateNodeChildren(line);
				tableCounter--;
				html.End(table);
			}
			else if(name.StartsWith("x_pane"))
			{
				var tr = html.Put(html.tr);
				trCounter++;
				CreateNodeChildren(line);
				trCounter--;
				html.End(tr);
			}
			else
			{
				ast.LineList lineList;
				if(nameDict.TryGetValue(name, out lineList))
				{
					CreateNode(lineList[0]);
				}
				else
				{
					parser.SemErr("Variable: '" + name + "'does NOT exist");
				}
			}
		}
		else if(node is ast.Final)
		{

		}
		else if(node is ast.Call)
		{
			var call = node as ast.Call;
			var value = call.path.name;
			if(value.Equals("dialog"))
			{
				var dialog = html.Put(html.dialog);
				html.Put(name);
				CreateNodeChildren(line);
				html.End(dialog);
			}
			else if(value.Equals("label"))
			{
				var label = html.Put(html.label);
				html.Put(name, label);
			}
			else if(value.Equals("edit"))
			{
				html.Write(html.edit);
			}
			else if(value.Equals("button"))
			{
				var button = html.Put(html.button);
				html.Put(name, button);
			}
			else
			{
				parser.SemErr("Variable: '" + value + "' does NOT exist");
			}
		}
	}
}
