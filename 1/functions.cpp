#include "functions.h"
#include <stdbool.h>

/* Convention. */
#define maximum_number_of_numbers_in_file 1024

size_t number_of_files = 0;
struct task *tasks;
static int current_task_i = 0;

#define check_resched {						 \
	int old_i = current_task_i;				 \
	current_task_i = (current_task_i + 1) % number_of_files; \
	if (setjmp(tasks[old_i].env) == 0)			 \
		longjmp(tasks[current_task_i].env, 1); }

int
compare(const void *a, const void *b) {
	return *(int*)a - *(int*)b;
}

void
sort_file(const char *file_name) {
	FILE *file;
	file = fopen(file_name, "r");
	if (!file) {
		printf("Can't open file.\n");
		return;
	}

	int buf = 0;
	int *sort_buffer = (int*)malloc(sizeof(int) * maximum_number_of_numbers_in_file);
	size_t number_of_numbers = 0;

	while (fscanf(file, "%d", &buf) != EOF) {
		sort_buffer[number_of_numbers] = buf;
		number_of_numbers++;
	}

	fclose(file);

	if (!number_of_numbers)
		return;

	qsort(sort_buffer, number_of_numbers, sizeof(int), compare);

	file = fopen(file_name, "wb");
	if (!file) {
		printf("Can't open file.\n");
		return;
	}

	for (size_t i = 0; i < number_of_numbers - 1; i++) {
		fprintf(file, "%d ", sort_buffer[i]);
	}

	fprintf(file, "%d", sort_buffer[number_of_numbers - 1]);
	fclose(file);

	free(sort_buffer);
}

void
coroutine()
{
	sort_file(tasks[current_task_i].file_name);
	check_resched;
	tasks[current_task_i].is_finished = true;
	while (true) {
		bool is_all_finished = true;
		for (int i = 0; i < number_of_files; ++i) {
			if (!tasks[i].is_finished) {
				is_all_finished = false;
				break;
			}
		}
		if (is_all_finished)
			return;
		check_resched;
	}
}

int
compare_pairs(const struct pair *a, const struct pair *b) {
	return a->key - b->key;
}
