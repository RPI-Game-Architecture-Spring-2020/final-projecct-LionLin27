/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_job.h"

#include "ga_condvar.h"
#include "ga_fiber.h"
#include "ga_intpool.h"
#include "ga_queue.h"

#include <atomic>
#include <thread>
#include <vector>

void* ga_job::_impl = 0;

struct ga_job_instance_t
{
	ga_job_instance_t() {}

	ga_job_decl_t* _decl;

	int32_t* _waiting_count;

	int _pool_index;

	ga_fiber _fiber;
	ga_fiber* _parent_fiber;
};

struct ga_job_system_impl_t
{
	ga_job_system_impl_t(int queue_size, int fiber_count) :
		_main_thread(std::this_thread::get_id()),
		_job_queue(queue_size),
		_job_instance_pool(fiber_count),
		_wait_queue(queue_size)
	{}

	std::thread::id _main_thread;

	ga_queue _job_queue;

	ga_intpool _job_instance_pool;
	ga_job_instance_t* _job_instance_data;

	ga_queue _wait_queue;

	std::vector<std::thread*> _worker_threads;

	ga_condvar _work_added;
	ga_condvar _work_exhausted;

	bool _terminate;
};

static int _ga_job_instance_thread_worker(void* data);
static bool _ga_job_schedule(ga_job_system_impl_t* impl, ga_fiber* parent_fiber);
static void _ga_job_run(ga_job_system_impl_t* impl, ga_fiber* parent_fiber, ga_job_instance_t* job);
static void _ga_job_fiber_worker(void* data);

void ga_job::startup(
	uint32_t hardware_thread_mask,
	int queue_size,
	int fiber_count)
{
	ga_job_system_impl_t* impl = new ga_job_system_impl_t(queue_size, fiber_count);

	impl->_terminate = false;

	impl->_job_instance_data = new ga_job_instance_t[fiber_count];
	for (int i = 0; i < fiber_count; ++i)
	{
		ga_job_instance_t* instance = &impl->_job_instance_data[i];
		instance->_fiber = ga_fiber(_ga_job_fiber_worker, instance, 64 * 1024);
		instance->_pool_index = i;
	}

	int hardware_thread_count = std::thread::hardware_concurrency();
	for (int i = 0; i < hardware_thread_count; ++i)
	{
		if ((hardware_thread_mask & (1 << i)) != 0)
		{
			impl->_worker_threads.push_back(new std::thread(_ga_job_instance_thread_worker, impl));
		}
	}

	_impl = impl;
}

void ga_job::shutdown()
{
	ga_job_system_impl_t* impl = static_cast<ga_job_system_impl_t*>(_impl);

	impl->_terminate = true;
	impl->_work_added.wake_all();
	for (auto& t : impl->_worker_threads)
	{
		t->join();
		delete t;
	}

	delete[] impl->_job_instance_data;
}

void ga_job::run(ga_job_decl_t* decls, int decl_count, int32_t* counter)
{
	*counter = decl_count;

	ga_job_system_impl_t* impl = static_cast<ga_job_system_impl_t*>(_impl);
	for (int i = 0; i < decl_count; ++i)
	{
		decls[i]._pending_count = counter;
		impl->_job_queue.push(decls + i);
	}

	impl->_work_added.wake_all();
}

void ga_job::wait(int32_t* counter)
{
	if (*counter > 0)
	{
		/*
		** If we're not the main thread, assume we're waiting from within a job.
		** In this case, push the current job onto the wait list, and reschedule.
		*/
		ga_job_system_impl_t* impl = static_cast<ga_job_system_impl_t*>(_impl);
		if (std::this_thread::get_id() != impl->_main_thread)
		{
			ga_job_instance_t* job = static_cast<ga_job_instance_t*>(ga_fiber::get_data());
			job->_waiting_count = counter;
			impl->_wait_queue.push(job);

			ga_fiber::switch_to(*job->_parent_fiber);
		}
		/*
		** Otherwise, in the main thread, block until jobs are complete.
		*/
		else
		{
			while (*counter > 0)
			{
				impl->_work_exhausted.wait();
			}
		}
	}
}

static int _ga_job_instance_thread_worker(void* data)
{
	ga_job_system_impl_t* impl = static_cast<ga_job_system_impl_t*>(data);

	ga_fiber parent_fiber = ga_fiber::convert_thread(0);

	while (!impl->_terminate)
	{
		if (!_ga_job_schedule(impl, &parent_fiber))
		{
			impl->_work_exhausted.wake_all();
			impl->_work_added.wait_for(1000);
		}
	}

	return 0;
}

static bool _ga_job_schedule(ga_job_system_impl_t* impl, ga_fiber* parent_fiber)
{
	/* Check for waiting jobs that are ready to run. */
	ga_job_instance_t* job;
	int retry_wait_count = impl->_wait_queue.get_count();
	while (retry_wait_count-- > 0 && impl->_wait_queue.pop((void**)&job))
	{
		if (job->_waiting_count == 0 || job->_waiting_count[0] == 0)
		{
			_ga_job_run(impl, parent_fiber, job);
			return true;
		}
		else
		{
			impl->_wait_queue.push(job);
		}
	}

	/* Look for queued jobs. */
	ga_job_decl_t* decl;
	if (impl->_job_queue.pop((void**)&decl))
	{
		int ga_job_index = impl->_job_instance_pool.alloc();

		job = &impl->_job_instance_data[ga_job_index];
		job->_decl = decl;
		job->_pool_index = ga_job_index;

		_ga_job_run(impl, parent_fiber, job);

		return true;
	}

	return impl->_wait_queue.get_count() != 0;
}

static void _ga_job_run(ga_job_system_impl_t* impl, ga_fiber* parent_fiber, ga_job_instance_t* job)
{
	job->_parent_fiber = parent_fiber;
	job->_waiting_count = 0;

	ga_fiber::switch_to(job->_fiber);

	if (job->_waiting_count == 0 || job->_waiting_count[0] == 0)
	{
		impl->_job_instance_pool.free(job->_pool_index);

		(*reinterpret_cast<std::atomic_int*>(job->_decl->_pending_count))--;
	}
}

static void _ga_job_fiber_worker(void* data)
{
	for (;;)
	{
		ga_job_instance_t* job = static_cast<ga_job_instance_t*>(ga_fiber::get_data());
		job->_decl->_entry(job->_decl->_data);
		ga_fiber::switch_to(*job->_parent_fiber);
	}
}
