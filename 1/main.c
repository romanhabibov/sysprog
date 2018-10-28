#include "functions.h"
#include <string.h>

extern size_t number_of_files;
extern struct task *tasks;
extern size_t starting_buf_size;

int
main(int argc, char *argv[]) {
	if (argc <= 1) {
		printf("No args.\n");
		return 0;
	}

	number_of_files = argc - 1;

	/* Memory allocation. */
	tasks = (struct task *)malloc(number_of_files * (sizeof(struct task)));
	int **tails = (int **)malloc(number_of_files * (sizeof(int *)));
	for (size_t i = 0; i < number_of_files; i++)
		tails[i] = (int *)malloc(starting_buf_size * sizeof(int));

	/* Sort files with coroutines. */
	for (size_t i = 0; i < number_of_files; i++) {
		tasks[i].buf_size = starting_buf_size;
		tasks[i].sort_buffer = tails[i];
		tasks[i].is_finished = false;
		tasks[i].time_normal = .0;
		strcpy(tasks[i].file_name, argv[i + 1]);
		tasks[i].number_of_numbers = 0;
		tasks[i].time_current = clock();
		setjmp(tasks[i].env);
	}
	coroutine();

	for (size_t i = 0; i < number_of_files; i++)
		free(tails[i]);
	free(tails);
	free(tasks);

	/* Merge sorted files into "result.txt". */
	struct pair *merge_buf \
	= (struct pair*)malloc(sizeof(struct pair) * number_of_files);
	FILE *file;
	int buf = 0;
	size_t already_read_files = 0;
	size_t invalid_files = 0;
	size_t i = 0;
	for (; i < number_of_files; i++) {
		file = fopen(argv[i + 1], "r");
		if (!file) {
			printf("Can't open file.\n");
			invalid_files++;
		}
		else if (fscanf(file, "%d", &buf) != EOF) {
			merge_buf \
			[i - already_read_files - invalid_files].key = buf;
			merge_buf \
			[i - already_read_files - invalid_files].value = file;
		}
		else {
			already_read_files++;
			fclose(file);
		}
	}

	size_t number_for_merge = i - already_read_files - invalid_files;
	FILE* result = fopen("result.txt", "wb");

	if (number_for_merge) {
		qsort(merge_buf, number_for_merge,
			sizeof(struct pair),
			(int(*) (const void*, const void*)) compare_pairs);

		i = 0;
		while (i < number_for_merge - 1) {
			fprintf(result, "%d ", merge_buf[i].key);
			if (fscanf(merge_buf[i].value, "%d", &merge_buf[i].key) \
				!= EOF) {
				qsort(merge_buf + i, number_for_merge - i,
					sizeof(struct pair),
					(int(*) (const void*, const void*)) \
					compare_pairs);
			}
			else
				i++;
		}

		fprintf(result, "%d ", merge_buf[i].key);

		/* Add last file, if any. */
		while (fscanf(merge_buf[i].value, "%d", &buf) \
			!= EOF)
			fprintf(result, "%d ", buf);
	}

	for (i = 0; i < number_for_merge; i++)
		fclose(merge_buf[i].value);
	free(merge_buf);
	fclose(result);

	printf("main %f\n", (double)(clock()) / CLOCKS_PER_SEC);
	getchar();
	return 0;
}
