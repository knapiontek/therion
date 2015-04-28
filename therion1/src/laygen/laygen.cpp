
#include "core/io/output.hpp"
#include "core/core/pager.hpp"
#include "core/core/list.hpp"
#include "laygen/dojo.hpp"
#include "laygen/laygen.hpp"

#include "laygen/cs.hpp"
#include "core/io/file.hpp"
#include "core/env/console.hpp"

class TypeSeq
{
public:
	enum Type
	{
		main, axis, xaxis, yaxis,
		dialog, label, edit, button,
		int8, int16, int32, int64,
		uint8, uint16, uint32, uint64,
		float32, float64, float128,
		nil
	};
public:
	static Type lookup(core::String& name)
	{
		static TypeSeq type_seq;
		Pair::share pair = type_seq.the_seq.lookup((Pair&)name);
		return core::is(pair)
			? pair->id
			: nil;
	}
private:
	struct Pair
	{
		typedef core::Shared<Pair> share;
		core::String name;
		Type id;
	};
private:
	TypeSeq()
	{
		static const core::String types[] = {
			"main", "axis", "x-axis", "y-axis",
			"dialog", "label", "edit", "button",
			"int8", "int16", "int32", "int64",
			"uint8", "uint16", "uint32", "uint64",
			"float32", "float64", "float128" };
		core::uint32 size = sizeof(types) / sizeof(core::String);
		the_seq.size(size);
		for(core::uint32 i = 0; i < size; i++)
		{
			Pair& pair = the_seq.at(i);
			pair.name = types[i];
			pair.id = (Type)i;
		}
		the_seq.sort();
	}
private:
	typedef core::CastIndex<core::String> StringIndex;
	core::Seq<Pair, StringIndex> the_seq;
};

class Build
{
public:
	static void laygen2dojo(core::String& laygen_filename, core::String& dojo_filename)
	{
		io::File dojo_file(dojo_filename);
		dojo_file.open(io::File::write_only, true);

		io::File laygen_file(laygen_filename);
		laygen_file.open(io::File::read_only);

		// read laygen file and add closing 0
		core::uint32 file_size = laygen_file.size();
		core::String laygen_content;
		laygen_content.size(1 + file_size); // 1 + for closing 0
		io::Decode io_decode(laygen_file.input());
		io_decode.read(laygen_content.data(), file_size);
		laygen_content.at(file_size) = 0;

		// parse
		laygen::Decode decode;
		core::String::Iterator it = laygen_content.head();
		while(it.next() && decode.put(it.value()));
		laygen::Model model = decode.model();
		if(!model)
			env::Fail(cs_syntax_error).arg(laygen_filename).arg(decode.line_cnt()).fire();

		Build build(laygen_filename, dojo_file.output());
		build.dojo(laygen_file.core(), model);

		dojo_file.close();
	}
private:
	struct Node
	{
		typedef core::Shared<Node> share;
		TypeSeq::Type type;
		laygen::Element element;
		core::Seq<Node> node_seq;
	};
private:
	Build(core::String& laygen_filename, io::Output& output)
		: the_laygen_filename(laygen_filename), the_encode(output)
	{

	}
	void dojo(core::String& file_guard, laygen::Model model)
	{
		core::uint32 tabs = 0;
		Node root;
		Node::share nodes[core::uint8_max];
		core::List<laygen::Element>::Iterator element_it = model->element_plus->head();
		while(element_it.next())
		{
			laygen::Element element = element_it.value();
			if(element->type_id)
			{
				Node::share parent;
				if(element->ln->tab_star)
				{
					core::uint32 element_tabs = element->ln->tab_star->size();
					if(element_tabs > tabs + 1)
						env::Fail(cs_max_tabs_allowed).arg(the_laygen_filename).arg(element->line).arg(tabs).fire();
					tabs = element_tabs;
					parent = nodes[tabs - 1];
				}
				else
				{
					tabs = 0;
					parent = root;
				}

				core::String type_name = laygen::cast(element->type_id);
				TypeSeq::Type type = TypeSeq::lookup(type_name);
				if(TypeSeq::nil == type)
					env::Fail(cs_unknown_type).arg(the_laygen_filename).arg(element->line).arg(type_name).fire();

				Node& node = parent->node_seq.append();
				node.type = type;
				node.element = element;
				nodes[tabs] = node;
			}
			else if(element->ln->tab_star)
				env::Fail(cs_trailing_tabs).arg(the_laygen_filename).arg(element->line).fire();
		}

		dojo::String content = the_build.acquire_String();
		content = acquire_node(root, content);
		dojo::Model dojo_model = the_build.acquire_Model(content);
		the_encode.write(dojo_model);
	}
	dojo::String acquire_node(Node& parent, dojo::String content)
	{
		dojo::String out = the_build.acquire_String();
		core::Seq<Node>::Iterator node_it = parent.node_seq.head();
		while(node_it.next())
		{
			Node& node = node_it.value();
			switch(node.type)
			{
			case TypeSeq::main:
				content = acquire_node(node, content);
				content = the_build.acquire_Main(content);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::axis:
				content = acquire_node(node, content);
				content = the_build.acquire_Axis(content);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::xaxis:
				content = acquire_node(node, content);
				content = the_build.acquire_XAxis(content);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::yaxis:
				content = acquire_node(node, content);
				content = the_build.acquire_YAxis(content);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::dialog:
				content = acquire_node(node, content);
				content = the_build.acquire_Dialog(content);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::label:
				content = the_build.acquire_Label(node.element->params_ques->string);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::edit:
				content = the_build.acquire_Edit(node.element->element_id);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::button:
				content = the_build.acquire_Button(node.element->params_ques->string);
				out->append(dojo::cast(content));
				break;
			case TypeSeq::int8:
				break;
			case TypeSeq::int16:
				break;
			case TypeSeq::int32:
				break;
			case TypeSeq::int64:
				break;
			case TypeSeq::uint8:
				break;
			case TypeSeq::uint16:
				break;
			case TypeSeq::uint32:
				break;
			case TypeSeq::uint64:
				break;
			case TypeSeq::float32:
				break;
			case TypeSeq::float64:
				break;
			case TypeSeq::float128:
				break;
			case TypeSeq::nil:
				core::assert(false);
			}
		}
		return out;
	}
private:
	core::String the_laygen_filename;
	dojo::Encode the_encode;
	dojo::Build the_build;
};

int main(int argc, char* argv[])
{
	env::init(argc, argv);
	try
	{
		bool verbose = false;
		core::String input_file;
		core::String output_file;

		for(core::int32 i = 1; i < argc; i++)
		{
			if(cs_verbose_switch.equal(argv[i]))
				verbose = true;
			else if(cs_input_switch.equal(argv[i]))
			{
				if(++i == argc)
					env::Fail::fire(cs_usage);
				input_file = argv[i];
			}
			else if(cs_output_switch.equal(argv[i]))
			{
				if(++i == argc)
					env::Fail::fire(cs_usage);
				output_file = argv[i];
			}
			else
				env::Fail::fire(cs_usage);
		}

		env::Log::verbose() = verbose;
		Build::laygen2dojo(input_file, output_file);
	}
	catch(env::Exception& exception)
	{
		env::Con::exception(exception);
	}
	catch(...)
	{
		env::Con::writeln(cs_unknown_exception);
	}
	return env::final();
}

