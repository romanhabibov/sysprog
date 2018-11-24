#include "thread_pool.h"
#include <pthread.h>

struct thread_task {
	thread_task_f function;
	void *arg;

	/* PUT HERE OTHER MEMBERS */
};

struct thread_pool {
	pthread_t *_threads;
	thread_task_f *queue;
	size_t _max_thread_count;
	size_t _created_thread_count;
	/* PUT HERE OTHER MEMBERS */
};

int
thread_pool_new(int max_thread_count, struct thread_pool *pool) {
	if (max_thread_count > TPOOL_MAX_THREADS || !max_thread_count)
		return TPOOL_ERR_INVALID_ARGUMENT;
	size_t size = sizeof(*pool) + max_thread_count * sizeof(pthread_t);
	pool = malloc(size);
	assert(pool);

	pool->_threads = (pthread_t *) pool + sizeof(*pool);
	pool->_max_thread_count = max_thread_count;
	pool->_created_thread_count = 0;
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
	pool->_threads[pool->_created_thread_count];
}

int
thread_task_new(struct thread_task **task, thread_task_f function, void *arg)
{
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
