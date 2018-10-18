#include "functions.h"
#include <stdbool.h>

size_t number_of_files = 0;
struct task *tasks;
static int current_task_i = 0;
static size_t old_i = 0;

#define check_resched {							 \
	size_t old_i = current_task_i;					 \
	do								 \
		current_task_i = (current_task_i + 1) % number_of_files; \
	while(tasks[current_task_i].is_finished);			 \
	if (setjmp(tasks[old_i].env) == 0)				 \
		longjmp(tasks[current_task_i].env, 1); }

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
		printf("Can't open file.\n");
		tasks[current_task_i].is_finished = true;
	}
	check_resched;

	int buf = 0;
	check_resched;

	while (fscanf(tasks[current_task_i].file, "%d", &buf) != EOF) {
		tasks[current_task_i].sort_buffer\
		[tasks[current_task_i].number_of_numbers] = buf;
		tasks[current_task_i].number_of_numbers++;
	}
	check_resched;

	fclose(tasks[current_task_i].file);
	check_resched;

	qsort(tasks[current_task_i].sort_buffer,
	      tasks[current_task_i].number_of_numbers,
	      sizeof(int), compare);
	check_resched;

	if (!tasks[current_task_i].number_of_numbers)
		tasks[current_task_i].is_finished = true;
	check_resched;

	tasks[current_task_i].file = fopen(tasks[current_task_i].file_name,
					   "wb");
	check_resched;

	for (size_t i = 0; i < tasks[current_task_i].number_of_numbers; i++)
		fprintf(tasks[current_task_i].file, "%d ",
			tasks[current_task_i].sort_buffer[i]);
	check_resched;

	fclose(tasks[current_task_i].file);

	tasks[current_task_i].is_finished = true;
	while (true) {
		bool is_all_finished = true;
		for (int i = 0; i < number_of_files; ++i) {
			if (!tasks[i].is_finished) {
				is_all_finished = false;
				break;
			}
		}
		if (is_all_finished) {
			return;
		}
		check_resched;
	}
}

int
compare_pairs(const struct pair *a, const struct pair *b) {
	return a->key - b->key;
}
