#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

/* Convention. */
#define maximum_number_of_numbers_in_file 1024

struct task {
	int id;
	jmp_buf env;
	bool is_finished;
	char file_name[128];
	FILE *file;
	size_t number_of_numbers;
	int sort_buffer[maximum_number_of_numbers_in_file];
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
