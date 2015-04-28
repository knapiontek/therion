using System;

class Dojo
{
	public static void Error(ast.Identity identity, string message)
	{
		Console.Out.WriteLine("{0}:{1}: {2}", identity.line, identity.col, message);
	}
	public static int Main(string[] args)
	{
		if(args.Length == 3)
		{
			var name = args[0];
			var input = args[1];
			var output = args[2];

			var root = new ast.Define();
			root.identity = new ast.Identity();
			root.identity.name = "root";
			var worker = new ast.Worker(root);

			var scanner = new coco.Scanner(input);
			var parser = new coco.Parser(scanner);

			parser.worker = worker;
			parser.Parse();

			if(parser.errors.count == 0)
			{
				var printer = new Printer(root);
				printer.Write(name, output);
			}

			Console.WriteLine(parser.errors.count + " errors detected");
			return parser.errors.count;
		}
		else
		{
			Console.WriteLine("name, input and output filename expected");
			return -1;
		}
	}
}
