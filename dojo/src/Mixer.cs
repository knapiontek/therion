
using System.IO;
using System.Collections.Generic;

class Mixer
{
	int level = 1;
	Stack<Node> stack;
	string filename;
	StreamWriter writer;
	const string tabs = "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

	internal class Node
	{
		public int index;
		public string value;
	}
	public void Dispose()
	{
		writer.Dispose();
	}
	public void Delete()
	{
		try
		{
			writer.Dispose();
			System.IO.File.Delete(filename);
		}
		catch (System.IO.IOException e)
		{
			System.Console.WriteLine("Fatal Error: " + e.Message);
		}
	}
	public void Open(string filename)
	{
		this.filename = filename;
		stack = new Stack<Node>();
		writer = new StreamWriter(filename);
	}
	public void Close()
	{
		while(stack.Count > 0)
		{
			var top = stack.Pop();
			var sub = top.value.Substring(top.index, top.value.Length - top.index);
			writer.Write(sub);
		}
		writer.Close();
	}
	public void Enter()
	{
		level++;
	}
	public void Leave()
	{
		level--;
	}
	public Node Put()
	{
		return Put("", null);
	}
	public Node Put(string text)
	{
		return Put(text, null);
	}
	public Node Put(string text, Node limit)
	{
		const string arg = "%%";
		const string at = "@@";
		const string line = "\n\t\t";
		if(text.StartsWith(at))
		{
			var ind = tabs.Substring(0, level);
			text = text.Replace(line, ind);
			text = text.Replace(at, ind);
		}
		int index = text.IndexOf(arg);
		if(index != -1)
		{
			var sub = text.Substring(0, index);
			writer.Write(sub);
			var node = new Node();
			node.index = index + arg.Length;
			node.value = text;
			stack.Push(node);
			return node;
		}
		else
		{
			writer.Write(text);
			Node top = null;
			while(stack.Count > 0)
			{
				top = stack.Peek();
				var i = top.value.IndexOf(arg, top.index);
				if(i != -1)
				{
					var sub = top.value.Substring(top.index, i - top.index);
					writer.Write(sub);
					top.index = i + arg.Length;
					return top;
				}
				else
				{
					var sub = top.value.Substring(top.index, top.value.Length - top.index);
					writer.Write(sub);
					top = stack.Pop();
					if(top == limit)
						return limit;
				}
			}
		}
		return null;
	}
	public void Write(string text)
	{
		const string at = "@@";
		const string line = "\n\t\t";
		if(text.StartsWith(at))
		{
			var ind = tabs.Substring(0, level);
			text = text.Replace(line, ind);
			text = text.Replace(at, ind);
		}
		writer.Write(text);
	}
	public void End(Node limit)
	{
		Node top = null;
		while(stack.Count > 0)
		{
			top = stack.Pop();
			var sub = top.value.Substring(top.index, top.value.Length - top.index);
			writer.Write(sub);
			if(top == limit)
				return;
		}
	}
}
