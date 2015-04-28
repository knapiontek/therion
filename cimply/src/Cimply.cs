
using System;

class Cimply
{
	public static int errorCount = 0;

	public static int Main(string[] args)
	{
		if(args.Length == 3)
		{
			var name = args[0];
			var input = args[1];
			var output = args[2];

			var root = ast.Define.Root(name);
			var worker = new ast.Worker(root);

			var scanner = new coco.Scanner(input);
			var parser = new coco.Parser(scanner);

			parser.worker = worker;
			parser.Parse();

			if(parser.errors.count == 0)
			{
				root.Evaluate(ast.Define.none);
				var printer = new Printer();
				printer.Write(root, output);
			}

			Console.WriteLine(parser.errors.count + errorCount + " errors detected");
			return parser.errors.count;
		}
		else
		{
			Console.WriteLine("name, input and output filename expected");
			return -1;
		}
	}
}
