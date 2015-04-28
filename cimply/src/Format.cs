
using System;
using System.IO;

class Final
{
	public const string arg = "%";
	public const string at = "@";
	public const string line = "\n\t\t";
	public const string tabs = "\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
}

class Format
{
	int index;
	string format;
	FormatWriter writer;

	public Format(FormatWriter writer)
	{
		index = 0;
		format = null;
		this.writer = writer;
	}
	public Format(string format, FormatWriter writer)
	{
		if(format.StartsWith(Final.at))
		{
			var indent = Final.tabs.Substring(0, writer.Level());
			format = format.Replace(Final.line, indent);
			format = format.Replace(Final.at, indent);
		}
		var first = format.IndexOf(Final.arg);
		var sub = format.Substring(0, first);
		writer.Write(sub);
		index = first + Final.arg.Length;
		this.format = format;
		this.writer = writer;
	}
	public Format Begin()
	{
		return new Format(writer);
	}
	public Format Begin(string format)
	{
		return new Format(format, writer);
	}
	public Format Put(string text)
	{
		if(text.StartsWith(Final.at))
		{
			var indent = Final.tabs.Substring(0, writer.Level());
			text = text.Replace(Final.line, indent);
			text = text.Replace(Final.at, indent);
		}
		writer.Write(text);
		return Put();
	}
	public Format Put()
	{
		if(format != null)
		{
			var next = format.IndexOf(Final.arg, index);
			if(next != -1)
			{
				var sub = format.Substring(index, next - index);
				writer.Write(sub);
				index = next + Final.arg.Length;
			}
			else
			{
				var sub = format.Substring(index, format.Length - index);
				writer.Write(sub);
			}
		}
		return this;
	}
	public void End()
	{
		var sub = format.Substring(index, format.Length - index);
		writer.Write(sub);
	}
}

class FormatWriter
{
	int level = 1;
	string filename;
	StreamWriter writer;

	public void Open(string filename)
	{
		this.filename = filename;
		writer = new StreamWriter(filename);
	}
	public void Close()
	{
		writer.Close();
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
			File.Delete(filename);
		}
		catch(System.IO.IOException exception)
		{
			Console.Out.WriteLine("Fatal Error: " + exception.Message);
		}
	}
	public void Write(string text)
	{
		writer.Write(text);
	}
	public void Enter()
	{
		level++;
	}
	public void Leave()
	{
		level--;
	}
	public int Level()
	{
		return level;
	}
	public Format Begin()
	{
		return new Format(this);
	}
	public Format Begin(string format)
	{
		return new Format(format, this);
	}
	public void Put(string text)
	{
		if(text.StartsWith(Final.at))
		{
			var indent = Final.tabs.Substring(0, level);
			text = text.Replace(Final.line, indent);
			text = text.Replace(Final.at, indent);
		}
		writer.Write(text);
	}
}
