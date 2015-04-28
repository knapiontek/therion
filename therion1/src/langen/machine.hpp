
#ifndef LANGEN_MACHINE_HPP
#define LANGEN_MACHINE_HPP

#include "langen/model.hpp"
#include "langen/cs.hpp"
#include "core/env/env.hpp"
#include "core/core/hashset.hpp"
#include "core/core/hashmap.hpp"
#include "core/core/treeset.hpp"
#include "core/core/treemap.hpp"

class Machine
{
public:
	struct Node
	{
		typedef core::Shared<Node> share;
		typedef core::TreeMap<core::uint32, Node::share> FollowMap;
		typedef core::HashMap<Model::Reduction::share, Node::share> ReductorMap;

		enum Info { conflict = 0x1, merge = 0x2, done = 0x4 };
		enum Flags { shift = 0x1, reduce = 0x2 };

		Node() : info(0), flags(shift), code(0)
		{

		}

		core::uint32 info;
		core::uint32 flags;
		core::uint32 code;
		core::String name;
		Model::Product::share product;
		FollowMap follow_map;
		ReductorMap reductor_map;
	};
	struct Package
	{
		core::uint32 start_offset;
		core::uint32 checkset_width;
		core::uint32 checkset_mask;
		core::List<core::uint32> list;
	};
public:
	Machine(Model& model) : the_model(model),
		the_acquire_list(0xFF),
		the_pack_set(0xFF),
		the_element_set(0x4F),
		the_match_map(0xFF)
	{

	}
	void create_tree(Node& root)
	{
		Node fake_reductor;
		fake_reductor.product = the_model.product;
		root.name = the_model.product->item->key;
		the_state_cnt = 0;

		load_reductions(root, fake_reductor);
		attach_reductors(root);
		match_nodes(the_model.product->id, root);
		apply_shift_and_reduce(root);
		count_nodes(root);
	}
	void pack_tree(Package& package, Node& root)
	{
		core::HashMap<core::uint32, core::uint32> index_map(0x8F);
		core::HashSet<core::uint32> index_set(0x8F);

		core::assert(-1 == the_model.product->id); // next line assumes it
		core::uint32 size = the_model.product_list.size() - 1 + the_model.input_map.size();

		package.list.page_size(0x4FF);
		create_pack_set(root);

		// compute nessessary checkset
		// compressed state machine has to verify the relation prev-next state for error checking
		// therefore we attach some lesser bits of prev to the next one for runtime comparision
		package.checkset_width = 1;
		while(size >> package.checkset_width)
			package.checkset_width++;
		package.checkset_mask = core::uint32_max >> (32 - package.checkset_width);

		// map of indices of nodes
		core::HashSet<Node::share>::Iterator node_it = the_pack_set.head();
		while(node_it.next())
		{
			Node& node = node_it.value();
			core::int32 index = -1;
			bool match;
			do
			{
				do
					index++;
				while(core::is(index_set.lookup(index))); // unique index
				match = true;
				core::int32 i = index + size - package.list.size();
				while(i-- > 0) // fill in empty cells
					package.list.append() = 0; // available
				Node::FollowMap::Sort follow_it = node.follow_map.sort();
				while(match && follow_it.next())
				{
					core::uint32 input_id = follow_it.key();
					match = !package.list.at(index + input_id); // test if taken
				}
			} while(!match);
			index_map.put(node.code, index);
			index_set.put(index);
			Node::FollowMap::Sort follow_it = node.follow_map.sort();
			while(follow_it.next())
			{
				core::uint32 input_id = follow_it.key();
				package.list.at(index + input_id) = 1; // taken
			}
		}

		// offset
		package.start_offset = index_map.at(0);
		package.start_offset <<= 2;
		package.start_offset |= Node::shift;

		// encode code and fill in 1-valued cells
		node_it = the_pack_set.head();
		while(node_it.next())
		{
			Node& node = node_it.value();
			core::uint32 index = index_map.lookup(node.code);
			Node::FollowMap::Sort follow_it = node.follow_map.sort();
			while(follow_it.next())
			{
				core::uint32 input_id = follow_it.key();
				Node& follow = follow_it.value();
				core::uint32 code = follow.flags == Node::shift
					? index_map.at(follow.code)
					: follow.code;
				code <<= 2;
				code |= follow.flags;
				code <<= package.checkset_width;
				code |= (index & package.checkset_mask);
				core::uint32& value = package.list.at(index + input_id);
				core::assert(1 == value);
				value = code;
			}
		}

		the_pack_set.erase_all();
	}
private:
	void load_reductions(Node& node, Node& reductor)
	{
		core::Seq<Model::Reduction>::Iterator reduction_it = reductor.product->reduction_seq.head();
		while(reduction_it.next())
		{
			Model::Reduction& reduction = reduction_it.value();
			Node::share link = node;

			core::Seq<Model::Element>::Iterator element_it = reduction.element_seq.head();
			while(element_it.next())
			{
				Model::Element& element = element_it.value();
				if(core::is(element.item->product))
				{
					if(!link->follow_map.page_size())
						link->follow_map.page_size(the_average_reduction_size);

					Node& leaf = link->follow_map.acquire(element.item->product->id, the_acquire_list);
					if(core::nil(leaf.name))
					{
						leaf.name = element.item->key;
						leaf.product = element.item->product;
						if(the_element_set.put(element))
						{
							load_reductions(link, leaf);
							the_element_set.erase(element);
						}
					}

					link = leaf;
				}
				else
				{
					core::String::Iterator byte_it = element.item->key.head();
					while(byte_it.next())
					{
						core::uint8 byte = byte_it.value();
						Model::Input& input = the_model.input_map.at(byte);
						if(!link->follow_map.page_size())
							link->follow_map.page_size(the_average_reduction_size);

						link = link->follow_map.acquire(input.id, the_acquire_list);
						if(core::nil(link->name))
							link->name = input.name;
					}
				}
			}

			if(!link->reductor_map.page_size())
				link->reductor_map.page_size(the_average_reduction_size);
			link->reductor_map.put(reduction, reductor);
		}
	}
	void attach_reductors(Node& node)
	{
		if(~node.info & Node::done)
		{
			node.info |= Node::done;
			if(node.reductor_map.page_size())
			{
				Node::ReductorMap::Iterator reductor_it = node.reductor_map.head();
				while(reductor_it.next())
				{
					Model::Reduction& reduction = reductor_it.key();
					Node& reductor = reductor_it.value();
					if(reductor.reductor_map.page_size())
						attach_reductors(reductor); // deal with empty reductions
					if(reductor.follow_map.page_size())
					{
						if(reduction.element_seq.size() // compression
							&& Model::Product::byte == reductor.product->clazz
							&& reductor.reductor_map.page_size()
							&& 1 == reductor.reductor_map.size())
						{
							Node::ReductorMap::Iterator it = reductor.reductor_map.head();
							it.next();
							Model::Reduction& reductor_reduction = it.key();
							attach_reductor_follows(node, reductor_reduction.id, reductor);
						}
						else
							attach_reductor_follows(node, reduction.id, reductor);
					}
				}
			}
			if(node.follow_map.page_size())
			{
				Node::FollowMap::Iterator follow_it = node.follow_map.head();
				while(follow_it.next())
				{
					Node& follow = follow_it.value();
					attach_reductors(follow);
				}
			}
		}
	}
	void attach_reductor_follows(Node& node, core::uint32 reduction_id, Node& reductor)
	{
		env::Log log;

		if(!node.follow_map.page_size())
			node.follow_map.page_size(the_average_reduction_size);

		Node::FollowMap::Iterator reductor_follow_it = reductor.follow_map.head();
		while(reductor_follow_it.next())
		{
			Node& reductor_follow = reductor_follow_it.value();
			if(core::nil(reductor_follow.product)
				|| Model::Product::byte == reductor_follow.product->clazz)
			{
				core::uint32 input_id = reductor_follow_it.key();
				Node& reductor_follow = reductor_follow_it.value();
				Node& node_follow = node.follow_map.acquire(input_id, the_acquire_list);

				if(core::nil(node_follow.name))
				{
					node_follow.flags = Node::reduce;
					node_follow.code = reduction_id;
					node_follow.name = reductor_follow.name;
				}
				else
				{
					node_follow.info |= Node::conflict;
					log.warn(cs_log_conflict)
						.arg(node.name)
						.arg(node_follow.name)
						.arg(node_follow.code)
						.arg(reduction_id)
						.end();
				}
			}
		}
	}
	Node& match_nodes(core::uint32 input_id, Node& node) // compression
	{
		if(node.follow_map.page_size())
		{
			core::TreeMap<core::uint32, Node::share>::Find it = the_match_map.find(input_id);
			while(it.next())
			{
				Node& peer = it.value();
				if(test_merge_nodes(node, peer))
				{
					merge_nodes(node, peer);
					if(node.follow_map.page_size())
						node.follow_map.erase_all();
					peer.info |= Node::merge;
					return peer;
				}
			}
			the_match_map.put(input_id, node, false);
			Node::FollowMap::Sort follow_it = node.follow_map.sort();
			while(follow_it.next())
			{
				core::uint32 follow_input_id = follow_it.key();
				Node::share& follow = follow_it.value();
				follow = match_nodes(follow_input_id, follow);
			}
		}
		return node;
	}
	bool test_merge_nodes(Node& src, Node& dst)
	{
		if(src.flags != dst.flags || src.code != dst.code)
			return false;
		if(src.follow_map.page_size() && dst.follow_map.page_size())
		{
			Node::FollowMap::Iterator follow_src_it = src.follow_map.head();
			Node::FollowMap::Iterator follow_dst_it = dst.follow_map.head();
			while(follow_src_it.next() && follow_dst_it.next())
			{
				core::uint32 input_id_src = follow_src_it.key();
				core::uint32 input_id_dst = follow_dst_it.key();
				Node& follow_src = follow_src_it.value();
				Node& follow_dst = follow_dst_it.value();
				if(Node::reduce & follow_src.flags & follow_dst.flags)
				{
					do
					{
						core::uint32 input_id = follow_src_it.key();
						Node& follow_src = follow_src_it.value();
						core::Shared<Node::share> follow_dst_exist = dst.follow_map.lookup(input_id);
						if(core::is(follow_dst_exist))
						{
							Node::share follow_dst = follow_dst_exist;
							if(!test_merge_nodes(follow_src, follow_dst))
								return false;
						}
					} while(follow_src_it.next());
					return true;
				}
				else if(input_id_src != input_id_dst || !test_merge_nodes(follow_src, follow_dst))
					return false;
			}
		}
		else if(src.follow_map.page_size() != dst.follow_map.page_size())
			return false;
		return true;
	}
	void merge_nodes(Node& src, Node& dst)
	{
		core::assert(src.flags == dst.flags && src.code == dst.code);

		if(src.follow_map.page_size() && dst.follow_map.page_size())
		{
			Node::FollowMap::Iterator follow_src_it = src.follow_map.head();
			while(follow_src_it.next())
			{
				core::uint32 input_id = follow_src_it.key();
				Node::share follow_src = follow_src_it.value();
				Node::share follow_dst = dst.follow_map.at(input_id, follow_src);
				if(follow_src != follow_dst)
					merge_nodes(follow_src, follow_dst);
			}
		}
	}
	void apply_shift_and_reduce(Node& node) // compression
	{
		env::Log log;
		if(node.follow_map.page_size())
		{
			if(node.reductor_map.page_size() && 1 == node.reductor_map.size())
			{
				core::uint32 flags = core::uint32_nil;
				core::uint32 code = core::uint32_nil;
				Node::FollowMap::Iterator follow_it = node.follow_map.head();
				if(follow_it.next())
				{
					Node& follow = follow_it.value();
					flags = follow.flags;
					code = follow.code;
					while(follow_it.next())
					{
						Node& follow = follow_it.value();
						if(flags != follow.flags || code != follow.code)
							break;
					}
					if(follow_it.tail())
					{
						node.info &= ~Node::merge;
						node.flags = Node::shift | Node::reduce;
						node.code = code;
					}
				}
			}
			Node::FollowMap::Iterator follow_it = node.follow_map.head();
			while(follow_it.next())
			{
				Node& follow = follow_it.value();
				apply_shift_and_reduce(follow);
			}
		}
		else if(node.reductor_map.page_size())
		{
			core::assert(1 == node.reductor_map.size());
			Node::ReductorMap::Iterator reductor_it = node.reductor_map.head();
			reductor_it.next();
			Model::Reduction& reduction = reductor_it.key();
			Node& reductor = reductor_it.value();
			if(!reductor.follow_map.page_size())
			{
				node.flags = Node::shift | Node::reduce;
				node.code = reduction.id;
				log.debug(cs_shift_reduce).arg(node.name).end();
			}
		}
	}
	void count_nodes(Node& node)
	{
		if(node.reductor_map.page_size())
			node.reductor_map.erase_all();
		if(node.follow_map.page_size() && node.follow_map.size())
		{
			node.code = the_state_cnt++;
			Node::FollowMap::Sort follow_it = node.follow_map.sort();
			while(follow_it.next())
			{
				Node& follow = follow_it.value();
				if(!follow.code)
					count_nodes(follow);
				else if(follow.follow_map.page_size() && Node::reduce & follow.flags)
					follow.follow_map.erase_all();
			}
		}
	}
	void create_pack_set(Node& node)
	{
		if(node.follow_map.page_size() && node.follow_map.size() && the_pack_set.put(node))
		{
			Node::FollowMap::Sort follow_it = node.follow_map.sort();
			while(follow_it.next())
			{
				Node& follow = follow_it.value();
				create_pack_set(follow);
			}
		}
	}
private:
	Model& the_model;
	core::uint32 the_state_cnt;
	core::List<Node> the_acquire_list;
	core::HashSet<Node::share> the_pack_set;
	core::HashSet<Model::Element::share> the_element_set;
	core::TreeMap<core::uint32, Node::share> the_match_map;
};

#endif // LANGEN_MACHINE_HPP
