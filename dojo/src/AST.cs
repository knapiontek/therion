
using System.Diagnostics;
using System.Collections.Generic;

namespace ast
{
	public class Identity
	{
		public string name;
		public int line, col;
	}
	public class Node
	{
		public static Node nil = new Node();

		public override string ToString()
		{
			return "nil";
		}
		public virtual string Type()
		{
			return ToString();
		}
	}
	public class Final : Node
	{
		public enum Kind
		{
			UNICODE, I1, I2, I4, I8, U1, U2, U4, U8, F8, F12,
			LAYOUT, DIALOG, PANE, TABPANE, EDIT, LABEL, BUTTON
		}
		public Kind kind;
		public string value;

		public override string ToString()
		{
			return value;
		}
		public override string Type()
		{
			switch(kind)
			{
			case Kind.UNICODE: return "root.unicode";
			case Kind.I1: return "root.int1";
			case Kind.I2: return "root.int2";
			case Kind.I4: return "root.int4";
			case Kind.I8: return "root.int8";
			case Kind.U1: return "root.uint1";
			case Kind.U2: return "root.uint2";
			case Kind.U4: return "root.uint4";
			case Kind.U8: return "root.uint8";
			case Kind.F8: return "root.float8";
			case Kind.F12: return "root.float12";
			default:
				Debug.Assert(false);
				return null;
			}
		}
	}
	public class NodeList : NilList<Node>
	{
		public NodeList() {}
		public NodeList(bool nil) : base(nil) {}
		public static NodeList nil = new NodeList(true);
	}
	public class Part
	{
		public Identity identity;
		public NodeList paramList = NodeList.nil;
		string name;

		public override string ToString()
		{
			if(name == null)
			{
				name = identity.name;
				if(paramList != NodeList.nil)
				{
					name += "(";
					string comma = null;
					foreach(var param in paramList)
					{
						name += comma + param.ToString();
						comma = ", ";
					}
					name += ")";
				}
			}
			return name;
		}
	}
	public class Path : Node
	{
		public List<Part> partList = new List<Part>();
		string name;

		public Identity DefaultName
		{
			get { return partList[partList.Count - 1].identity; }
		}
		public override string ToString()
		{
			if(name == null)
			{
				string dot = null;
				foreach(var part in partList)
				{
					name += dot + part.ToString();
					dot = ".";
				}
			}
			return name;
		}
	}
	public class DefineList : NilList<Define>
	{
		public DefineList() {}
		public DefineList(bool nil) : base(nil) {}
		public static DefineList nil = new DefineList(true);
	}
	public class Define
	{
		public const int PARAM = 1;
		public Identity identity;
		public DefineList paramList = DefineList.nil;
		public Node node = Node.nil;
		public DefineList childList = DefineList.nil;
		public int flag;
		public int unique;
		string name;
		string type;

		public override string ToString()
		{
			if(name == null)
			{
				name = identity.name;
				if(paramList != DefineList.nil)
				{
					name += "(";
					string comma = null;
					foreach(var param in paramList)
					{
						name += comma + param.ToString();
						comma = ", ";
					}
					name += ")";
				}
				if(0 != (flag & PARAM))
					name += ": " + node.ToString();
			}
			return name;
		}
		public string Type()
		{
			if(type == null)
			{
				type = identity.name;
				if(paramList != DefineList.nil)
				{
					type += "(";
					string comma = null;
					foreach(var param in paramList)
					{
						type += comma + param.Type();
						comma = ", ";
					}
					type += ")";
				}
			}
			return type;
		}
	}
	public class Scope : Node
	{
		DefineList list = new DefineList();

		public Scope(Define root)
		{
			list.Add(root);
		}
		public Scope(Scope scope)
		{
			list.AddRange(scope.list);
		}
		public Define Enter(Define define)
		{
			var leaf = list[list.Count - 1];
			if(0 != (define.flag & ast.Define.PARAM))
				list[list.Count - 1] = define;
			else
				list.Add(define);
			return leaf;
		}
		public void Leave(Define define)
		{
			if(0 != (define.flag & ast.Define.PARAM))
				list[list.Count - 1] = define;
			else
				list.RemoveAt(list.Count - 1);
		}
		public void Leave()
		{
			list.RemoveAt(list.Count - 1);
		}
		public void Add(Define define)
		{
			list.Add(define);
		}
		public int Level
		{
			set { list.RemoveRange(value + 1, list.Count - value - 1); }
			get { return list.Count - 1; }
		}
		public Define Leaf
		{
			set { list[list.Count - 1] = value; }
			get { return list[list.Count - 1]; }
		}
		public override string ToString()
		{
			string name = null;
			string dot = null;
			foreach(var define in list)
			{
				name += dot + define.ToString();
				dot = ".";
			}
			return name;
		}
		public override string Type()
		{
			string type = null;
			string dot = null;
			foreach(var define in list)
			{
				type += dot + define.Type();
				dot = ".";
			}
			return type;
		}
	}
	public class Worker
	{
		Scope scope;

		public Worker(Define root)
		{
			scope = new Scope(root);
		}
		public void Add(Define define)
		{
			var leaf = scope.Leaf;
			var found = leaf.childList.FindLast(element => element.identity.name == define.identity.name);
			if(found == null)
				define.unique = 0;
			else
			{
				if(found.unique == 0)
					found.unique = 1;
				define.unique = found.unique + 1;
			}
			if(leaf.childList == DefineList.nil)
				leaf.childList = new DefineList();
			leaf.childList.Add(define);
			scope.Add(define);
		}
		public bool Level(int level)
		{
			if(level < scope.Level)
				scope.Level = level;
			return level > scope.Level;
		}
	}
}
