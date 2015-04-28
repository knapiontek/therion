
public class SafeList<Type> : System.Collections.Generic.List<Type>
{
	bool mutable;

	public SafeList()
	{
		this.mutable = true;
	}
	public SafeList(bool mutable)
	{
		this.mutable = mutable;
	}
	public bool IsMutable
	{
		get { return mutable; }
	}
	public new void Insert(int index, Type item)
	{
		if(mutable)
			base.Insert(index, item);
		else
			throw new System.NotImplementedException();
	}
	public new void RemoveAt(int index)
	{
		if(mutable)
			base.RemoveAt(index);
		else
			throw new System.NotImplementedException();
	}
	public new void Add(Type item)
	{
		if(mutable)
			base.Add(item);
		else
			throw new System.NotImplementedException();
	}
	public new void AddRange(System.Collections.Generic.IEnumerable<Type> collection)
	{
		if(mutable)
			base.AddRange(collection);
		else
			throw new System.NotImplementedException();
	}
	public new void Clear()
	{
		if(mutable)
			base.Clear();
		else
			throw new System.NotImplementedException();
	}
	public new bool Remove(Type item)
	{
		if(mutable)
			return base.Remove(item);
		else
			throw new System.NotImplementedException();
	}
}
