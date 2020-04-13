/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_queue.h"

#include <atomic>
#include <cstdint>

struct ga_queue_nodecount_t
{
	uint32_t _index;
	uint32_t _count;
};

union ga_queue_pointer_t
{
	std::atomic<uint64_t> _atomic;

	uint64_t _entire;
	ga_queue_nodecount_t _part;

	ga_queue_pointer_t() {}
	ga_queue_pointer_t(const ga_queue_pointer_t& other) : _entire(other._entire) {}
	ga_queue_pointer_t& operator=(const ga_queue_pointer_t& other) { _entire = other._entire; return *this; }
};

struct ga_queue_node_t
{
	void* _data;
	ga_queue_pointer_t _next;
};

struct ga_queue_impl_t
{
	ga_queue_pointer_t _head;
	ga_queue_pointer_t _tail;
	ga_queue_pointer_t _free_list;

	std::atomic_int _count;

	ga_queue_node_t* _nodes;
};

static const uint32_t k_ga_queue_invalid_index = 0xffffffff;

static uint32_t _alloc_node_index(ga_queue_impl_t* impl);
static void _free_node_index(ga_queue_impl_t* impl, uint32_t index);
static ga_queue_node_t* _init_node(ga_queue_impl_t* impl, uint32_t node_index);

ga_queue::ga_queue(int node_count)
{
	auto impl = new ga_queue_impl_t;

	impl->_count = 0;
	impl->_free_list._entire = 0;
	impl->_nodes = new ga_queue_node_t[node_count];

	/* Link nodes together. */
	for (int i = 0; i < node_count - 1; ++i)
	{
		impl->_nodes[i]._next._part._index = i + 1;
		impl->_nodes[i]._next._part._count = 0;
	}
	impl->_nodes[node_count - 1]._next._part._index = k_ga_queue_invalid_index;
	impl->_nodes[node_count - 1]._next._part._count = 0;

	/* Populate the queue with a dummy node. */
	uint32_t dummy_index = _alloc_node_index(impl);
	_init_node(impl, dummy_index);

	impl->_head._part._index = dummy_index;
	impl->_head._part._count = 0;
	impl->_tail._part._index = dummy_index;
	impl->_tail._part._count = 0;

	_impl = impl;
}

ga_queue::~ga_queue()
{
	ga_queue_impl_t* impl = static_cast<ga_queue_impl_t*>(_impl);
	delete[] impl->_nodes;
	delete impl;
}

void ga_queue::push(void* data)
{
	ga_queue_impl_t* impl = static_cast<ga_queue_impl_t*>(_impl);

	/* Allocate a new node for this data. */
	uint32_t node_index = _alloc_node_index(impl);
	ga_queue_node_t* node = _init_node(impl, node_index);
	node->_data = data;

	ga_queue_pointer_t tail;

	/* Try until the push succeeds. */
	for (;;)
	{
		tail = impl->_tail;
		ga_queue_pointer_t next = impl->_nodes[tail._part._index]._next;

		/* Is our view of the queue still consistent? If not, try again. */
		if (tail._entire == impl->_tail._entire)
		{
			/* Is tail pointing to last node? */
			if (next._part._index == k_ga_queue_invalid_index)
			{
				/* Attempt to push new node onto tail. Leave the loop on success. */
				ga_queue_pointer_t link;
				link._part._index = node_index;
				link._part._count = next._part._count + 1;
				if (impl->_nodes[tail._part._index]._next._atomic.compare_exchange_strong(next._entire, link._entire))
				{
					break;
				}
			}

			/* Tail has fallen behind the actual end of the queue. Fix that. */
			else
			{
				ga_queue_pointer_t link;
				link._part._index = next._part._index;
				link._part._count = tail._part._count + 1;
				impl->_tail._atomic.compare_exchange_strong(tail._entire, link._entire);
			}
		}
	}

	/* Try to advance the tail pointer. We'll handle the fail case on future calls. */
	{
		ga_queue_pointer_t link;
		link._part._index = node_index;
		link._part._count = tail._part._count + 1;
		impl->_tail._atomic.compare_exchange_strong(tail._entire, link._entire);
		impl->_count++;
	}
}

bool ga_queue::pop(void** data)
{
	ga_queue_impl_t* impl = static_cast<ga_queue_impl_t*>(_impl);
	ga_queue_pointer_t head;

	for (;;)
	{
		head = impl->_head;
		ga_queue_pointer_t tail = impl->_tail;
		ga_queue_pointer_t next = impl->_nodes[head._part._index]._next;

		/* Is our view of the queue still consistent? If not, try again. */
		if (head._entire == impl->_head._entire)
		{
			if (head._part._index == tail._part._index)
			{
				/* If queue is empty, fail the pop. */
				if (next._part._index == k_ga_queue_invalid_index)
				{
					return false;
				}

				/* Tail has fallen behind the actual end of the queue. Fix that. */
				ga_queue_pointer_t link;
				link._part._index = next._part._index;
				link._part._count = tail._part._count + 1;
				impl->_tail._atomic.compare_exchange_strong(tail._entire, link._entire);
			}
			else
			{
				/* Grab the data. */
				*data = impl->_nodes[next._part._index]._data;

				/* Attempt to pop the node. Leave the loop on success. */
				ga_queue_pointer_t link;
				link._part._index = next._part._index;
				link._part._count = head._part._count + 1;
				if (impl->_head._atomic.compare_exchange_strong(head._entire, link._entire))
				{
					break;
				}
			}
		}
	}

	impl->_count--;
	_free_node_index(impl, head._part._index);
	return true;
}

int ga_queue::get_count() const
{
	ga_queue_impl_t* impl = static_cast<ga_queue_impl_t*>(_impl);
	return impl->_count;
}

static uint32_t _alloc_node_index(ga_queue_impl_t* impl)
{
	uint32_t index;

	for (;;)
	{
		ga_queue_pointer_t free_list = impl->_free_list;

		if (free_list._part._index != k_ga_queue_invalid_index)
		{
			index = free_list._part._index;
			ga_queue_pointer_t next = impl->_nodes[index]._next;

			ga_queue_pointer_t link;
			link._part._index = next._part._index;
			link._part._count = free_list._part._count + 1;
			if (impl->_free_list._atomic.compare_exchange_strong(free_list._entire, link._entire))
			{
				break;
			}
		}
	}

	return index;
}

static void _free_node_index(ga_queue_impl_t* impl, uint32_t index)
{
	ga_queue_node_t* node = impl->_nodes + index;
	for (;;)
	{
		ga_queue_pointer_t free_list = impl->_free_list;
		node->_next._part._index = free_list._part._index;

		ga_queue_pointer_t link;
		link._part._index = index;
		link._part._count = free_list._part._count + 1;
		if (impl->_free_list._atomic.compare_exchange_strong(free_list._entire, link._entire))
		{
			break;
		}
	}
}

static ga_queue_node_t* _init_node(ga_queue_impl_t* impl, uint32_t node_index)
{
	ga_queue_node_t* node = impl->_nodes + node_index;
	node->_data = 0;
	node->_next._part._index = k_ga_queue_invalid_index;
	node->_next._part._count = 0;

	std::atomic_thread_fence(std::memory_order_release);

	return node;
}
