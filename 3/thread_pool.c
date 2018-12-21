#include "thread_pool.h"
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

struct thread_task {
	thread_task_f _function;
	void *_returned_code;
	int _status;
	struct thread_pool *_pool;
	struct thread_task *_next;
	void *_arg;
};

struct thread_pool {
	size_t _threads_size;
	size_t _created_thread_count;
	size_t _proceed_thread_count;
	size_t _idle_thread_count;
	size_t _tasks_count;
	pthread_mutex_t _pool_data_mutex;
	pthread_cond_t _pool_cond_var;
	struct thread_task *_head;
	struct thread_task *_tail;
	pthread_t *_threads;
	/* PUT HERE OTHER MEMBERS */
};

int
thread_pool_new(int max_thread_count, struct thread_pool **pool)
{
	if (max_thread_count > TPOOL_MAX_THREADS || !max_thread_count)
		return TPOOL_ERR_INVALID_ARGUMENT;
	size_t size = sizeof(**pool) + max_thread_count * sizeof(pthread_t);
	(*pool) = malloc(size);
	assert(*pool);

	(*pool)->_threads = (pthread_t *) pool + sizeof(**pool);
	(*pool)->_head = NULL;
	(*pool)->_tail = NULL;
	(*pool)->_pool_data_mutex;
	(*pool)->_threads_size = max_thread_count;
	(*pool)->_created_thread_count = 0;
	(*pool)->_proceed_thread_count = 0;
	(*pool)->_idle_thread_count = 0;
	(*pool)->_tasks_count = 0;
	return 0;
}

int
thread_pool_thread_count(const struct thread_pool *pool)
{
	return pool->_created_thread_count;
}

int
thread_pool_delete(struct thread_pool *pool)
{
	if (pool->_head || pool->_proceed_thread_count)
		return TPOOL_ERR_HAS_TASKS;
	free(pool);
	return 0;
}

static void *
task_runner(void *arg)
{
	struct thread_task *thread = (struct thread_task *) arg;
	pthread_mutex_lock(&(thread->_pool->_pool_data_mutex));
	if (thread->_pool->_idle_thread_count)
		thread->_pool->_idle_thread_count--;
	thread->_pool->_proceed_thread_count++;
	pthread_mutex_unlock(&(thread->_pool->_pool_data_mutex));
	thread->_returned_code = thread->_function(thread->_arg);
	thread->_status = FINISHED;
	pthread_mutex_lock(&(thread->_pool->_pool_data_mutex));
	thread->_pool->_proceed_thread_count--;
	thread->_pool->_idle_thread_count++;
	pthread_mutex_unlock(&(thread->_pool->_pool_data_mutex));
	for(;;) {
		pthread_mutex_lock(&(thread->_pool->_pool_data_mutex));
		while(!NULL)
			pthread_cond_wait(&(thread->_pool->_pool_cond_var), &(thread->_pool->_pool_data_mutex));
		/* Grab new task.*/
		thread = thread->_pool->_head;
		thread->_pool->_head = thread->_next;
		thread->_pool->_tasks_count--;
		pthread_mutex_unlock(&(thread->_pool->_pool_data_mutex));
		thread->_returned_code = thread->_function(thread->_arg);
		thread->_status = FINISHED;
	}
}

int
thread_pool_push_task(struct thread_pool *pool, struct thread_task *task)
{
	if (pool->_tasks_count == TPOOL_MAX_TASKS)
		return TPOOL_ERR_TOO_MANY_TASKS;
	task->_pool = pool;
	if (!pool->_created_thread_count) {
		pthread_create(&(pool->_threads[0]), NULL, &task_runner, (void *) task);
		pool->_created_thread_count++;
	} else {
		if ((pool->_idle_thread_count) ||
			(pool->_created_thread_count < pool->_threads_size)) {
			pthread_mutex_lock(&(pool->_pool_data_mutex));
			if (!pool->_head) {
				pool->_head = task;
				pool->_tail = task;
			} else {
				pool->_tail->_next = task;
				pool->_tail = task;
			}
			pool->_tasks_count++;
			pthread_cond_signal(&(pool->_pool_cond_var));
			pthread_mutex_unlock(&(pool->_pool_data_mutex));
		} else {
			pthread_create(&(pool->_threads[pool->_created_thread_count]), NULL, &task_runner, (void *) task);
			pool->_created_thread_count++;
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
	(*task)->_function = function;
	(*task)->_returned_code = NULL;
	(*task)->_status = NOT_PUSHED;
	return 0;
	/* IMPLEMENT THIS FUNCTION */
}

bool
thread_task_is_finished(const struct thread_task *task)
{
	if (task->_status == NOT_PUSHED)
		return TPOOL_ERR_TASK_NOT_PUSHED;
	else if (task->_status == FINISHED)
		return true;
	else
		return false;
	/* IMPLEMENT THIS FUNCTION */
}

bool
thread_task_is_running(const struct thread_task *task)
{
	if (task->_status == NOT_PUSHED)
		return TPOOL_ERR_TASK_NOT_PUSHED;
	else if (task->_status == PROCEED)
		return true;
	else
		return false;
	/* IMPLEMENT THIS FUNCTION */
}

int
thread_task_join(struct thread_task *task, void **result)
{
	if (task->_status == NOT_PUSHED)
		return TPOOL_ERR_TASK_NOT_PUSHED;
	while (task->_status != FINISHED) {}
	(*result) = task->_returned_code;
	return 0;
	/* IMPLEMENT THIS FUNCTION */
}

int
thread_task_delete(struct thread_task *task)
{
	if ((task->_status == PENDING) ||
		(task->_status == PROCEED))
		return TPOOL_ERR_TASK_IN_POOL;
	free(task);
	/* IMPLEMENT THIS FUNCTION */
}
