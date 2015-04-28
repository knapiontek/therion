
public class NilList<Type> : System.Collections.Generic.List<Type>
{
	bool nil;

	public NilList()
	{
		this.nil = false;
	}
	public NilList(bool nil)
	{
		this.nil = true;
	}
	public new void Insert(int index, Type item)
	{
		if(nil)
			throw new System.NotImplementedException();
		else
			base.Insert(index, item);
	}
	public new void RemoveAt(int index)
	{
		if(nil)
			throw new System.NotImplementedException();
		else
			base.RemoveAt(index);
	}
	public new void Add(Type item)
	{
		if(nil)
			throw new System.NotImplementedException();
		else
			base.Add(item);
	}
	public new void AddRange(System.Collections.Generic.IEnumerable<Type> collection)
	{
		if(nil)
			throw new System.NotImplementedException();
		else
			base.AddRange(collection);
	}
	public new void Clear()
	{
		if(nil)
			throw new System.NotImplementedException();
		else
			base.Clear();
	}
	public new bool Remove(Type item)
	{
		if(nil)
			throw new System.NotImplementedException();
		else
			return base.Remove(item);
	}
}
