
class JS : Mixer
{
	public string header =
		@"@@dojo.require(""dijit.Dialog"");
		dojo.require(""dijit.form.TextBox"");
		dojo.require(""dijit.form.TimeTextBox"");
		dojo.require(""dijit.form.Button"");
		dojo.require(""dijit.form.DateTextBox"");
		dojo.require(""dijit.layout.ContentPane"");
		dojo.require(""dijit.layout.BorderContainer"");

		nil = null;
		unicode = """";
		int1 = 0;
		int2 = 0;
		int4 = 0;
		int8 = 0;
		uint1 = 0;
		uint2 = 0;
		uint4 = 0;
		uint8 = 0;
		float4 = 0.0;
		float8 = 0.0;
		float12 = 0.0;
		byte = uint1;
		int = int4;
		uint = uint4;
		float = float8;

		that = this;
		heap = [];
		begin() {
			heap.push(that);
			that = {};
		}
		begin(name) {
			heap.push(that);
			that = that[name] = {};
		}
		end() {
			var end = that;
			that = heap.pop();
			return end;
		}

		dojo.ready(function() {%%
		});";
	public string comma = ", ";
	public string proto =
		@"@@that[""%%""] = function(%%) {%%
		}";
	public string protoClazz =
		@"@@begin();%%
		return end();";
	public string clazz =
		@"@@begin(""%%"");%%
		end();";
	public string keyValue = @"@@that[""%%""] = %%;";
	public string keyClone = @"@@that[""%%""] = dojo.clone(%%);";
	public string Dojo(string value)
	{
		switch(value)
		{
		case "layout": return @"@@%%.dojo = new dijit.layout.BorderContainer(%%);";
		case "pane": return @"@@%%.dojo = new dijit.layout.ContentPane(%%);";
		case "dialog": return @"@@%%.dojo = new dijit.Dialog(%%);";
		case "label": return @"@@%%.dojo = new dijit.form.Label(%%);";
		case "edit": return @"@@%%.dojo = new dijit.form.TextBox(%%);";
		case "button": return @"@@%%.dojo = new dijit.form.Button(%%);";
		default: return null;
		}
	}
	public string DojoChild(string value)
	{
		switch(value)
		{
		case "layout": return @"@@%%.dojo.addChild(%%.dojo);";
		case "pane": return @"@@%%.dojo.addChild(%%.dojo);";
		case "dialog": return @"@@%%.dojo.addChild(%%.dojo);";
		case "label": return @"@@%%.dojo.addChild(%%.dojo);";
		case "edit": return @"@@%%.dojo.addChild(%%.dojo);";
		case "button": return @"@@%%.dojo.addChild(%%.dojo);";
		default: return null;
		}
	}
}
