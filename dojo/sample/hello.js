dojo.require("dijit.Dialog");
dojo.require("dijit.form.TextBox");
dojo.require("dijit.form.TimeTextBox");
dojo.require("dijit.form.Button");
dojo.require("dijit.form.DateTextBox");

function profileDialog(){
	return dijit.byId("profileDialog");
}
function showDialog(){
	profileDialog().show();
}
function hideDialog(){
	profileDialog().hide();
}
