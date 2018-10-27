#include "functions.h"

size_t number_of_files = 0;
struct task *tasks;
static int current_task_i = 0;
static size_t old_i = 0;
size_t starting_buf_size = 100000;

#define check_resched {							 \
	tasks[current_task_i].time_normal += (double)(clock() - tasks[current_task_i].time_current) / CLOCKS_PER_SEC; \
	size_t old_i = current_task_i;					 \
		current_task_i = (current_task_i + 1) % number_of_files; \
	tasks[current_task_i].time_current = clock(); \
	if (setjmp(tasks[old_i].env) == 0)				 \
		longjmp(tasks[current_task_i].env, 1); }

#define is_all { \
	while (true) { \
		bool is_all_finished = true; \
		for (int i = 0; i < number_of_files; ++i) { \
			if (!tasks[i].is_finished) { \
				is_all_finished = false; \
				break; \
			} \
		} \
		if (is_all_finished) { \
			return; \
		} \
		check_resched; \
		} }

int
compare(const void *a, const void *b) {
	return *(int*)a - *(int*)b;
}

void
coroutine()
{
	tasks[current_task_i].file = fopen(tasks[current_task_i].file_name,
		"r");
	check_resched;
	if (tasks[current_task_i].file == NULL) {
		check_resched;
		printf("Can't open file.\n");
		check_resched;
		tasks[current_task_i].is_finished = true;
		printf("#%d %f\n", current_task_i, tasks[current_task_i].time_normal);
		is_all;
	}
	check_resched;

	while (fscanf(tasks[current_task_i].file, "%d",
		      &tasks[current_task_i].current_num) != EOF) {
		check_resched;
		if (tasks[current_task_i].buf_size == tasks[current_task_i].number_of_numbers) {
			check_resched;
			tasks[current_task_i].buf_size += starting_buf_size;
			check_resched;
			tasks[current_task_i].sort_buffer \
			= (int *)realloc(tasks[current_task_i].sort_buffer,
					 (tasks[current_task_i].buf_size \
					 + starting_buf_size) * sizeof(int));
			check_resched;
		}
		check_resched;
		tasks[current_task_i].sort_buffer[tasks[current_task_i]\
		.number_of_numbers] = tasks[current_task_i].current_num;
		check_resched;
		tasks[current_task_i].number_of_numbers++;
		check_resched;
	}
	check_resched;

	fclose(tasks[current_task_i].file);
	check_resched;

	qsort(tasks[current_task_i].sort_buffer,
		tasks[current_task_i].number_of_numbers,
		sizeof(int), compare);
	check_resched;

	if (!tasks[current_task_i].number_of_numbers) {
		check_resched;
		tasks[current_task_i].is_finished = true;
		printf("#%d %f\n", current_task_i,
		       tasks[current_task_i].time_normal);
		is_all;
	}
	check_resched;

	tasks[current_task_i].file = fopen(tasks[current_task_i].file_name,
		"wb");
	check_resched;

	tasks[current_task_i].i = 0;
	check_resched;

	while (tasks[current_task_i].i < tasks[current_task_i].number_of_numbers) {
		check_resched;
		fprintf(tasks[current_task_i].file, "%d ",
			tasks[current_task_i].sort_buffer[tasks[current_task_i].i]);
		check_resched;
		tasks[current_task_i].i++;
		check_resched;
	}
	check_resched;

	fclose(tasks[current_task_i].file);

	tasks[current_task_i].is_finished = true;
	printf("#%d %f\n", current_task_i, tasks[current_task_i].time_normal);

	is_all;
}

int
compare_pairs(const struct pair *a, const struct pair *b) {
	return a->key - b->key;
}
