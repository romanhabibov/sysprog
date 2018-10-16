#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

struct task {
	int id;
	jmp_buf env;
	bool is_finished;
	char file_name[128];
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
