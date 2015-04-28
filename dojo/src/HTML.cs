
class HTML : Mixer
{
	public string body =
		@"@@<!DOCTYPE HTML>
		<html lang='en'>
		<head>
		<meta charset='utf-8'/>
		<title>%%</title>
		<link rel='stylesheet' href='http://ajax.googleapis.com/ajax/libs/dojo/1.6/dijit/themes/claro/claro.css'/>
		<script src='http://ajax.googleapis.com/ajax/libs/dojo/1.6/dojo/dojo.xd.js' data-dojo-config='parseOnLoad: false'></script>
		<script src='hello.js'></script>
		</head>
		<body class='claro'>
		<h1>Profile</h1>
		<div class='dijitHidden'>%%
		</div>
		</body>
		</html>";
	public string table =
		@"@@<table>%%
		</table>";
	public string tr =
		@"@@<tr>%%
		</tr>";
	public string td =
		@"@@<td>%%
		</td>";
	public string dialog =
		@"@@<div data-dojo-type='dijit.Dialog' data-dojo-props='title:""profile_dialog""' id='%%'>
			<p>Hello From Kris:</p>%%
		</div>";
	public string label =
		@"@@<label data-dojo-type='dijit.form.Label' data-dojo-props='onClick:hide_dialog'>%%</label>";
	public string edit =
		@"@@<input data-dojo-type='dijit.form.TextBox' data-dojo-props='onClick:hide_dialog'></input>";
	public string button =
		@"@@<button data-dojo-type='dijit.form.Button' data-dojo-props='onClick:hide_dialog'>%%</button>";
}
