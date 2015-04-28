
using System.Collections.Generic;

class Iterator
{
	List<Solver> list = new List<Solver>();
	int major;
	int minor;
	
	public Iterator(Solver solver)
	{
		list.Add(solver);
		var node = solver.EvaluateNode(solver.Leaf.node);
		while(node is Solver)
		{
			solver = node as Solver;
			list.Add(solver);
			node = solver.EvaluateNode(solver.Leaf.node);
		}
		major = list.Count - 1;
		minor = -1;
	}
	public bool IsProto
	{
		get { return list[0].Leaf.paramList != ast.DefineList.nil; }
	}
	public bool IsComplex
	{
		get { return -1 != list.FindIndex(solver => solver.Leaf.childList.Count > 0); }
	}
	public ast.Define Object
	{
		get { return list[0].Leaf; }
	}
	public string Variable
	{
		get
		{
			var last = list.FindLast(solver => solver.Leaf.flag == ast.Define.PARAM);
			return (last != null) ? last.Leaf.Type() : Value;
		}
	}
	public string Value
	{
		get { return list[list.Count - 1].Leaf.node.ToString(); }
	}
	public bool Next
	{
		get
		{
			++minor;
			while(major >= 0 && minor == list[major].Leaf.childList.Count)
			{
				--major;
				minor = 0;
			}
			return major >= 0;
		}
	}
	public Iterator Enter()
	{
		var solver = list[major];
		var child = solver.Leaf.childList[minor];
		solver.Enter(child);
		return new Iterator(solver);
	}
	public void Leave()
	{
		list[major].Leave();
	}
}
