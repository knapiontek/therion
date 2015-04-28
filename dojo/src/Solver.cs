
using System.Collections.Generic;

class Solver : ast.Scope
{
	ast.Node limit;
	Solver context;
	List<ast.Identity> list;
	static List<ast.Identity> nillable = new List<ast.Identity>();

	public Solver(ast.Define root) : base(root)
	{
		list = nillable;
	}
	public Solver(ast.Node node, Solver context) : base(context)
	{
		limit = node;
		this.context = context;
		list = nillable;
	}
	public ast.Node EvaluateNode(ast.Node node)
	{
		if(node is ast.Path)
		{
			var path = node as ast.Path;
			var part = path.partList[0];
			var solver = new Solver(node, this);
			while(!solver.EvaluatePart(part))
			{
				if(0 == solver.Level)
					return solver.Error(part);
				solver.Leave();
			}
			for(var i = 1; i < path.partList.Count; i++)
			{
				part = path.partList[i];
				if(!solver.EvaluatePart(part))
					return solver.Error(part);
			}
			return solver;
		}
		return node;
	}
	bool EvaluatePart(ast.Part part)
	{
		if(Accept(part))
		{
			if(Leaf.paramList != ast.DefineList.nil)
			{
				var clone = new ast.Define();
				clone.identity = part.identity;
				clone.paramList = new ast.DefineList();
				for(var i = 0; i < Leaf.paramList.Count; i++)
				{
					var param = new ast.Define();
					param.identity = Leaf.paramList[i].identity;
					param.node = context.EvaluateNode(part.paramList[i]);
					clone.paramList.Add(param);
				}
				clone.node = Leaf.node;
				clone.childList = Leaf.childList;
				Leaf = clone;
			}
			return true;
		}
		return false;
	}
	bool Accept(ast.Part part)
	{
		foreach(var param in Leaf.paramList)
		{
			if(Accept(part, param))
				return true;
		}
		if(Leaf != context.Leaf)
		{
			foreach(var child in Leaf.childList)
			{
				if(child.node != limit && Accept(part, child))
					return true;
			}
			var node = EvaluateNode(Leaf.node);
			while(node is Solver)
			{
				var super = node as Solver;
				foreach(var child in super.Leaf.childList)
				{
					if(Accept(part, child))
						return true;
				}
				node = super.EvaluateNode(super.Leaf.node);
			}
		}
		return false;
	}
	bool Accept(ast.Part part, ast.Define define)
	{
		if(part.identity.name == define.identity.name)
		{
			var leaf = Enter(define);
			Trace();
			var success = (part.paramList.Count == define.paramList.Count);
			for(var i = 0; success && i < part.paramList.Count; i++)
			{
				var key = context.EvaluateNode(part.paramList[i]);
				var hole = EvaluateNode(define.paramList[i].node);
				success = key.Type() == hole.Type();
				while(!success && key is Solver)
				{
					var super = key as Solver;
					key = super.EvaluateNode(super.Leaf.node);
					success = key.Type() == hole.Type();
				}
			}
			if(success)
				return true;
			Leave(leaf);
		}
		return false;
	}
	void Trace()
	{
		if(list == nillable)
			list = new List<ast.Identity>();
		var identity = new ast.Identity();
		identity.name = ToString();
		identity.line = Leaf.identity.line;
		identity.col = Leaf.identity.col;
		list.Add(identity);
	}
	ast.Node Error(ast.Part part)
	{
		Dojo.Error(part.identity, "Not found: " + part.ToString() + " in: " + context.ToString());
		foreach(var candidate in list)
			Dojo.Error(candidate, "\tCandidate: " + candidate.name);
		return ast.Node.nil;
	}
}
