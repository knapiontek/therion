
using System;
using System.Diagnostics;
using System.Collections.Generic;

namespace ast
{
	public class Identity
	{
		public static int count = 0;
		public static Identity none = new Identity();

		public string val;
		public string cval;
		public int line, col;

		public Identity()
		{
			val = "none" + ++count;
			cval = val;
			line = 0;
			col = 0;
		}
		public Identity(string val)
		{
			this.val = val;
			cval = val;
			line = 0;
			col = 0;
		}
		public Identity(string val, int line, int col)
		{
			this.val = val;
			cval = val.Replace("-", "_") + ++count;
			this.line = line;
			this.col = col;
		}
		public Identity(string val, string cval, int line, int col)
		{
			this.val = val;
			this.cval = cval + ++count;
			this.line = line;
			this.col = col;
		}
		public Identity Clone()
		{
			return new Identity(val, line, col);
		}
	}
	public class Link
	{
		public static Link nil = new Link();

		public virtual Link Clone()
		{
			return this;
		}
		public virtual Define Find(Define from)
		{
			return Define.nil;
		}
		public virtual Define Tail
		{
			get { return Define.nil; }
		}
		public virtual int Level
		{
			get { return 0; }
		}
		public override string ToString()
		{
			return Define.nil.identity.val;
		}
	}
	public class Final : Link
	{
		public string value;
		public Define define;

		public Final(Define define)
		{
			value = define.identity.val;
			this.define = define;
		}
		public Final(string value, Define define)
		{
			this.value = value;
			this.define = define;
		}
		public override Define Find(Define from)
		{
			return define;
		}
		public override Define Tail
		{
			get { return define; }
		}
		public override string ToString()
		{
			return value;
		}
	}
	public class LinkList : SafeList<Link>
	{
		public static LinkList none = new LinkList(false);
		public static LinkList empty = new LinkList(false);

		public LinkList() {}
		public LinkList(bool mutable) : base(mutable) {}

		public LinkList Clone()
		{
			if(IsMutable)
			{
				var linkList = new LinkList();
				foreach(var link in this)
					linkList.Add(link.Clone());
				return linkList;
			}
			return this;
		}
	}
	public class Part
	{
		public Identity identity;
		public LinkList paramList = LinkList.none;

		public Define define = Define.nil;
		public int level = 0;

		public void Bind(Define accept, bool clone)
		{
			level += accept.Level;
			if(clone && (accept.flag & Define.BUILTIN) == 0)
			{
				define = accept.Clone();
				for(var i = 0; i < paramList.Count; i++)
					define.paramList[i].link = paramList[i];
				if(define.that.cloneList == DefineList.none)
					define.that.cloneList = new DefineList();
				define.that.cloneList.Add(define);
				define.Evaluate(define.that);
			}
			else
			{
				define = accept;
			}
		}
		public Part Clone()
		{
			var part = new Part();
			part.identity = identity;
			part.paramList = paramList.Clone();
			part.level = level;
			return part;
		}
		public string Name
		{
			get { return identity.cval; }
		}
		public override string ToString()
		{
			var val = identity.val;
			if(paramList != LinkList.none)
			{
				val += "(";
				if(paramList != LinkList.empty)
				{
					val += paramList[0].ToString();
					for(int i = 1; i < paramList.Count; i++)
						val += ", " + paramList[i].ToString();
				}
				val += ")";
			}
			return val;
		}
	}
	public class Cache
	{
		public Part part;
		public Define from;
		public DefineList candidateList = DefineList.none;

		public Cache(Part part, Define from)
		{
			this.part = part;
			this.from = from;
		}
		public void Watch(Define candidate)
		{
			if(candidateList == DefineList.none)
				candidateList = new DefineList();
			candidateList.Add(candidate);
		}
		public Define MatchError()
		{
			Cimply.errorCount++;
			Log(part.identity, "Error: cannot match: " + part.ToString() + " in: " + from.ToString());
			foreach(var candidate in candidateList)
				Log(candidate.identity, "\tCandidate: " + candidate.ToString());
			return Define.nil;
		}
		public void LevelError()
		{
			Cimply.errorCount++;
			Log(part.identity, "Error: operator '$' does not match: " + part.ToString() + " in: " + from.ToString());
		}
		public void Debug(Define define)
		{
			Log(part.identity, "Entity: " + part.ToString() + " in: " + define.ToString());
		}
		void Log(Identity identity, string message)
		{
			Console.Out.WriteLine("{0}:{1}: {2}", identity.line, identity.col, message);
		}
		public override string ToString()
		{
			return part.ToString() + " in: " + from.ToString();
		}
	}
	public class Path : Link
	{
		bool done = false;
		public List<Part> partList = new List<Part>();

		public override Link Clone()
		{
			var path = new Path();
			foreach(var part in partList)
				path.partList.Add(part.Clone());
			return path;
		}
		public override Define Find(Define from)
		{
			if(done == false)
			{
				done = true;
				var define = from;
				var first = partList[0];
				var cache = new Cache(first, from);
				while(!define.Populate(cache))
				{
					define = define.that;
					if(define == Define.none)
						return cache.MatchError();
				}
				for(var i = 1; i < partList.Count; i++)
				{
					var tail = partList[i - 1];
					cache = new Cache(partList[i], from);
					if(tail.level != 0)
						cache.LevelError();
					if(!tail.define.Populate(cache))
						return cache.MatchError();
					if(tail.level < 0)
						cache.LevelError();
				}
				for(var i = 1; i < partList.Count; i++)
				{
					var tail = partList[i - 1].define;
					var that = partList[i].define.that;
					while(tail.Type != that)
					{
						tail = tail.Type.link.Tail;
						var item = new Part();
						item.identity = tail.identity;
						item.define = tail;
						partList.Insert(i++, item);
					}
				}
				foreach(var part in partList)
				{
					for(var i = 0; i < part.paramList.Count; i++)
					{
						var path = part.paramList[i] as Path;
						if(path != null)
						{
							var tail = path.Tail.Type;
							var expect = part.define.paramList[i].link.Tail.Type;
							while(tail != expect)
							{
								tail = tail.link.Tail.Type;
								tail.flag |= Define.REFERRED;
								var item = new Part();
								item.identity = tail.identity;
								item.define = tail;
								path.partList.Add(item);
							}
						}
					}
				}
				if(first.define.that.that != Define.none)
				{
					define = from;
					while(define != first.define.that)
					{
						define.flag |= Define.NEED_THAT;
						define = define.that;
					}
				}
				foreach(var part in partList)
				{
					part.define.flag |= Define.REFERRED;
					if((from.flag & Define.SNAP) != 0)
						part.define.flag |= Define.SNAPPED;
				}
			}
			return partList[partList.Count - 1].define;
		}
		public override Define Tail
		{
			get { return partList[partList.Count - 1].define; }
		}
		public override int Level
		{
			get { return partList[partList.Count - 1].level; }
		}
		public override string ToString()
		{
			string val = partList[0].ToString();
			for(int i = 1; i < partList.Count; i++)
				val += "." + partList[i].ToString();
			return val;
		}
	}
	public class DefineList : SafeList<Define>
	{
		public static DefineList none = new DefineList(false);
		public static DefineList empty = new DefineList(false);

		public DefineList() {}
		public DefineList(bool mutable) : base(mutable) {}
		
		public DefineList Clone()
		{
			if(IsMutable)
			{
				var defineList = new DefineList();
				foreach(var define in this)
					defineList.Add(define.Clone());
				return defineList;
			}
			return this;
		}
	}
	public class Define
	{
		public static Define none = Final("none");
		public static Define nil = Final("nil");
		public static Define boolean = Final("bool");
		public static Define int1 = Final("int1");
		public static Define int2 = Final("int2");
		public static Define int4 = Final("int4");
		public static Define int8 = Final("int8");
		public static Define uint1 = Final("uint1");
		public static Define uint2 = Final("uint2");
		public static Define uint4 = Final("uint4");
		public static Define uint8 = Final("uint8");
		public static Define float8 = Final("float8");
		public static Define float12 = Final("float12");
		public static Define unicode = Final("unicode");

		public const uint FINAL = 1 << 0;
		public const uint BUILTIN = 1 << 1;
		public const uint PARAM = 1 << 2;
		public const uint REFERRED = 1 << 3;
		public const uint SNAPPED = 1 << 4;
		public const uint NEED_COPY = 1 << 5;
		public const uint NEED_THAT = 1 << 6;
		public const uint CASE = 1 << 7;
		public const uint SNAP = 1 << 8;
		public const uint UNTIL = 1 << 9;
		public const uint FOREACH = 1 << 10;

		public Identity identity = Identity.none;
		public DefineList paramList = DefineList.none;
		public Link link = Link.nil;
		public DefineList fieldList = DefineList.none;

		public Define that;
		public uint flag = 0;
		public DefineList cloneList = DefineList.none;

		static Define()
		{
			none.that = none;
		}
		public static Define Root(string name)
		{
			var define = new Define();
			define.identity = new Identity(name);
			define.flag = REFERRED;
			return define;
		}
		public static Define Final(string name)
		{
			var define = new Define();
			define.identity = new Identity(name);
			define.flag = FINAL;
			return define;
		}
		public static Define BuiltIn(string name)
		{
			var define = new Define();
			define.identity = new Identity(name);
			define.flag = BUILTIN;
			return define;
		}
		public static Define BuiltIn(string name, Final input, Final output)
		{
			var define = BuiltIn(name);
			var param1 = BuiltIn("p1");
			var param2 = BuiltIn("p2");
			define.link = output;
			param1.link = input;
			param2.link = input;
			define.paramList = new DefineList();
			define.paramList.Add(param1);
			define.paramList.Add(param2);
			return define;
		}
		public Define Clone()
		{
			var define = new Define();
			define.identity = identity.Clone();
			define.paramList = paramList.Clone();
			define.link = link.Clone();
			define.fieldList = fieldList.Clone();
			define.that = that;
			define.flag = flag;
			return define;
		}
		public void Evaluate(Define that)
		{
			this.that = that;
			foreach(var param in paramList)
			{
				param.flag |= PARAM;
				param.Evaluate(that);
			}
			link.Find(this);
			foreach(var field in fieldList)
				field.Evaluate(this);
		}
		public bool Populate(Cache cache)
		{
			cache.Debug(this);
			foreach(var param in paramList)
				if(Accept(param, cache))
					return true;
			if(this != cache.from)
			{
				foreach(var clone in cloneList)
					if(Accept(clone, cache))
						return true;
				foreach(var field in fieldList)
					if(Accept(field, cache))
						return true;
				var define = link.Find(this).Type;
				while(define != nil)
				{
					foreach(var param in define.paramList)
						if(Accept(param, cache))
							return true;
					foreach(var clone in define.cloneList)
						if(Accept(clone, cache))
							return true;
					foreach(var field in define.fieldList)
						if(Accept(field, cache))
							return true;
					define = define.link.Find(define).Type;
				}
			}
			return false;
		}
		bool Accept(Define define, Cache cache)
		{
			if(define.identity.val == cache.part.identity.val)
			{
				cache.Watch(define);
				if(define.paramList.Count == cache.part.paramList.Count)
				{
					var clone = false;
					for(var i = 0; i < define.paramList.Count; i++)
					{
						var keyLink = cache.part.paramList[i];
						var holeLink = define.paramList[i].link;
						var key = keyLink.Find(cache.from).Type;
						var hole = holeLink.Find(define).Type;
						if(keyLink.Level != holeLink.Level)
							return false;
						while(key != hole)
						{
							if(key == nil)
								return false;
							key = key.link.Find(key).Type;
							clone = true;
						}
					}
					cache.part.Bind(define, clone);
					return true;
				}
			}
			return false;
		}
		public Define Call
		{
			get
			{
				var define = this;
				while(define.link.Tail != nil && define.paramList == DefineList.none && define.fieldList == DefineList.none)
					define = define.link.Tail;
				return define;
			}
		}
		public Define Type
		{
			get
			{
				var define = this;
				while(define.link.Tail != nil && define.fieldList == DefineList.none)
					define = define.link.Tail;
				return define;
			}
		}
		public bool IsCall
		{
			get { return paramList != DefineList.none; }
		}
		public bool IsType
		{
			get { return fieldList != DefineList.none; }
		}
		public bool IsFinal
		{
			get
			{
				var define = this;
				while(define != nil)
				{
					if(define.fieldList != DefineList.none)
						return false;
					define = define.link.Tail;
				}
				return true;
			}
		}
		public bool IsRoot
		{
			get { return that == none; }
		}
		public bool IsPrintable
		{
			get { return (flag & (FINAL | BUILTIN)) == 0 && (link != Link.nil || paramList != DefineList.none || fieldList != DefineList.none); }
		}
		public bool IsSpecial
		{
			get { return (flag & (CASE | SNAP | UNTIL | FOREACH)) != 0; }
		}
		public bool IsBuiltIn
		{
			get { return (flag & BUILTIN) != 0; }
		}
		public bool IsParam
		{
			get { return (flag & PARAM) != 0; }
		}
		public bool IsReferred
		{
			get { return (flag & REFERRED) != 0; }
		}
		public bool IsSnapped
		{
			get { return (flag & SNAPPED) != 0; }
		}
		public bool NeedThat
		{
			get { return (flag & NEED_THAT) != 0; }
		}
		public bool NeedCopy
		{
			get { return (flag & NEED_COPY) != 0; }
		}
		public bool IsCase
		{
			get { return (flag & CASE) != 0; }
		}
		public bool IsSnap
		{
			get { return (flag & SNAP) != 0; }
		}
		public bool IsUntil
		{
			get { return (flag & UNTIL) != 0; }
		}
		public bool IsForeach
		{
			get { return (flag & FOREACH) != 0; }
		}
		public int Level
		{
			get { return paramList == DefineList.none && fieldList == DefineList.none ? link.Level : 0; }
		}
		public string Name
		{
			get { return identity.cval; }
		}
		public override string ToString()
		{
			var val = PartToString();
			var define = that;
			while(define != none)
			{
				val = define.PartToString() + "." + val;
				define = define.that;
			}
			return val;
		}
		string PartToString()
		{
			var val = identity.val;
			if(paramList != DefineList.none)
			{
				val += "(";
				if(paramList != DefineList.empty)
				{
					val += paramList[0].link.ToString();
					for(int i = 1; i < paramList.Count; i++)
						val += ", " + paramList[i].link.ToString();
				}
				val += ")";
			}
			return val;
		}
	}
	public class Worker
	{
		DefineList list = new DefineList();

		public Worker(Define root)
		{
			var boolean = new Final(Define.boolean);
			root.fieldList = new DefineList();
			root.fieldList.Add(Define.nil);
			AddOps(root, Define.boolean, boolean);
			AddOps(root, Define.int1, boolean);
			AddOps(root, Define.int2, boolean);
			AddOps(root, Define.int4, boolean);
			AddOps(root, Define.int8, boolean);
			AddOps(root, Define.uint1, boolean);
			AddOps(root, Define.uint2, boolean);
			AddOps(root, Define.uint4, boolean);
			AddOps(root, Define.uint8, boolean);
			AddOps(root, Define.float8, boolean);
			AddOps(root, Define.float12, boolean);
			AddOps(root, Define.unicode, boolean);
			list.Add(root);
		}
		void AddOps(Define root, Define define, Final boolean)
		{
			var final = new Final(define);
			root.fieldList.Add(define);
			root.fieldList.Add(Define.BuiltIn("+", final, final));
			root.fieldList.Add(Define.BuiltIn("<<", final, final));
			root.fieldList.Add(Define.BuiltIn(">>", final, final));
			root.fieldList.Add(Define.BuiltIn("<", final, boolean));
			root.fieldList.Add(Define.BuiltIn(">", final, boolean));
			root.fieldList.Add(Define.BuiltIn("<=", final, boolean));
			root.fieldList.Add(Define.BuiltIn(">=", final, boolean));
			if(define != Define.unicode)
			{
				if(define != Define.float8 && define != Define.float12)
				{
					root.fieldList.Add(Define.BuiltIn("&", final, final));
					root.fieldList.Add(Define.BuiltIn("|", final, final));
					root.fieldList.Add(Define.BuiltIn("^", final, final));
					root.fieldList.Add(Define.BuiltIn("%", final, final));
				}
				root.fieldList.Add(Define.BuiltIn("*", final, final));
				root.fieldList.Add(Define.BuiltIn("/", final, final));
				root.fieldList.Add(Define.BuiltIn("-", final, final));
			}
		}
		public void Enter(Define define)
		{
			var that = list[list.Count - 1];
			if(that.fieldList == DefineList.none)
				that.fieldList = new DefineList();
			that.fieldList.Add(define);
			list.Add(define);
		}
		public bool Level(int level)
		{
			if(level < list.Count - 1)
				list.RemoveRange(level + 1, list.Count - 1 - level);
			return level > list.Count - 1;
		}
	}
}
