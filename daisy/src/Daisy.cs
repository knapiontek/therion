using System;

class Daisy
{
	public static void Main(string[] args)
	{
		if(args.Length == 3)
		{
			var name = args[0];
			var input = args[1];
			var output = args[2];
	
			var scanner = new coco.Scanner(input);
			var parser = new coco.Parser(scanner);
			var worker = new ast.Worker(parser);

			parser.worker = worker;
			parser.Parse();

			var builder = new Builder(parser);
			builder.WriteAssembly(name, output, worker.Root);

			Console.WriteLine(parser.errors.count + " errors detected");
		}
		else
		{
			Console.WriteLine("name, input and output filename expected");
		}
	}
}
