
using System;
using System.Collections.Generic;

namespace ast
{
	public enum Keyword
	{
		NONE, RETURN, FOREACH, WHILE, REPEAT, UNTIL, THROW, TRACE, CATCH
	}
	public class Op
	{
		public enum Kind { ASSIGN, ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, NOT, NUM, INC, DEC, SHL, SHR }
		public enum Flag { PRE, BIN, POST }
		public Kind kind;
		public Flag flag;
	}
	public class Part
	{
		public string name;
		public int line, col;
		public List<Op> opList = new List<Op>();
	}
	public class Path
	{
		public List<Part> partList = new List<Part>();
	}
	public class ExprList : List<Expr>
	{
		public static ExprList empty = new ExprList();
	}
	public class Item
	{
		public Op op;
	}
	public class Const : Item
	{
		public enum Kind { STRING, I1, I2, I4, I8, U1, U2, U4, U8, F4, F8 }
		public Kind kind;
		public string value;
	}
	public class Call : Item
	{
		public Path path;
		public ExprList exprList = ExprList.empty;
	}
	public class Expr : Item
	{
		public List<Item> itemList = new List<Item>();
	}
	public class Line
	{
		public enum Kind { CODE, CLASS, FUNCTION }
		public Kind kind;
		public Keyword keyword;
		public Expr expr;
		public List<Line> childList = new List<Line>();
	}
	public class Worker
	{
		coco.Parser parser;
		int indentation;
		Line[] stack;

		public Worker(coco.Parser parser)
		{
			this.parser = parser;
			indentation = 0;
			stack = new Line[0xff];
			stack[0] = new Line();
		}
		public void Indentation(int indentation)
		{
			if(this.indentation + 1 < indentation)
				parser.SemErr("Inconsistent indentation (\\t ...)");
			this.indentation = indentation;
		}
		public void Add(Keyword keyword, Expr expr)
		{
			var line = new Line();
			line.kind = Line.Kind.CODE;
			line.keyword = keyword;
			line.expr = expr;
			stack[indentation + 1] = line;

			var root = stack[indentation];
			root.childList.Add(line);
			if(root.kind != Line.Kind.FUNCTION)
			{
				if(line.keyword == Keyword.RETURN)
					root.kind = Line.Kind.FUNCTION;
				else
					root.kind = Line.Kind.CLASS;
			}
		}
		public Line Root
		{
			get { return stack[0]; }
		}
	}
}
