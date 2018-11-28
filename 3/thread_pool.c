#include "thread_pool.h"
#include <pthread.h>
#include <stdbool.h>

struct thread_task {
	int _id;
	thread_task_f _function;
	int _status;
	bool _is_running;
	void *_arg;
	/* PUT HERE OTHER MEMBERS */
};

struct thread_pool {
	size_t _threads_size;
	size_t _active_thread_count;
	size_t _created_thread_count;
	size_t _pending_tasks_count;
	size_t _queue_head_offset;
	size_t _queue_size;
	pthread_mutex_t _pool_data_mutex;
	thread_task_f *_queue;
	pthread_t *_threads;
	/* PUT HERE OTHER MEMBERS */
};

int
thread_pool_new(int max_thread_count, struct thread_pool **pool) {
	if (max_thread_count > TPOOL_MAX_THREADS || !max_thread_count)
		return TPOOL_ERR_INVALID_ARGUMENT;
	size_t size = sizeof(**pool) + max_thread_count * sizeof(pthread_t);
	(*pool) = malloc(size);
	assert(*pool);

	(*pool)->_threads = (pthread_t *) pool + sizeof(**pool);
	(*pool)->_queue = NULL;
	(*pool)->_pool_data_mutex;
	(*pool)->_threads_size = max_thread_count;
	(*pool)->_active_thread_count = 0;
	(*pool)->_created_thread_count = 0;
	(*pool)->_pending_tasks_count = 0;
	(*pool)->_queue_head_offset = 0;
	(*pool)->_queue_size = 0;
	size_t _pending_tasks_count;
	return 0;
}

int
thread_pool_thread_count(const struct thread_pool *pool) {
	return pool->_created_thread_count;
}

int
thread_pool_delete(struct thread_pool *pool) {
	if (pool->_created_thread_count)
		return TPOOL_ERR_HAS_TASKS;
	free(pool);
	return 0;
}

int
thread_pool_push_task(struct thread_pool *pool, struct thread_task *task) {
	if (!pool->_created_thread_count) {
		pthread_create(pool->_threads[0], task->_function, task->_arg);
		pool->_created_thread_count++;
	} else if (pool->_created_thread_count < pool->_threads_size) {
		if (pool->_active_thread_count < pool->_created_thread_count) {

		} else {
			pthread_create(pool->_threads[0], task->_function, task->_arg);
			pool->_created_thread_count++;
		}
	} else {
		if (pool->_active_thread_count < pool->_created_thread_count) {

		} else {
			if (!pool->_queue_size) {
				pool->_queue = malloc(sizeof(thread_task_f) * 512);
				pool->_queue_size = 500;
			} else if (pool->_queue_head_offset + pool->_pending_tasks_count == pool->_queue_size) {
				pool->_queue = realloc(pool->_queue, sizeof(thread_task_f) * (pool->_queue_size + 512));
				pool->_queue_size += 512;
			}
			pool->_queue[pool->_pending_tasks_count]; // task id?
			pool->_pending_tasks_count++;
		}
	}

	return 0;
}

int
thread_task_new(struct thread_task **task, thread_task_f function, void *arg)
{
	size_t size = sizeof(**task) + sizeof(*arg);
	(*task) = malloc(size);
	assert(*task);

	(*task)->_arg = (void *) arg + sizeof(**task);
	(*task)->_status = NOT_PUSHED;
	return 0;
	/* IMPLEMENT THIS FUNCTION */
}

bool
thread_task_is_finished(const struct thread_task *task)
{
	/* IMPLEMENT THIS FUNCTION */
}

bool
thread_task_is_running(const struct thread_task *task)
{
	if (!task) {
		if (task->_status)
			return task->_is_running;
		else
	/* IMPLEMENT THIS FUNCTION */
}

int
thread_task_join(struct thread_task *task, void **result)
{
	/* IMPLEMENT THIS FUNCTION */
}

int
thread_task_delete(struct thread_task *task)
{
	/* IMPLEMENT THIS FUNCTION */
}
