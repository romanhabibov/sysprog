#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct task {
	size_t number_of_numbers;
	int current_num;
	size_t i;
	double time_normal;
	clock_t time_current;
	jmp_buf env;
	bool is_finished;
	char file_name[128];
	FILE *file;
	int *sort_buffer;
};

struct pair {
	int key;
	FILE* value;
};

int
compare(const void *a, const void *b);

void
sort_file(const char *file_name);

void
coroutine();

int
compare_pairs(const struct pair *a, const struct pair *b);
