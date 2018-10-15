#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Convention.
#define maximum_number_of_numbers_in_file 1024

int compare(const void *a, const void *b) {
	return *(int*)a - *(int*)b;
}

void sort_file(const char *file_name) {
	FILE *file;
	file = fopen(file_name, "r");
	if (file == NULL) {
		printf("Can't open file\n");
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

	qsort(sort_buffer, number_of_numbers, sizeof(int), compare);

	file = fopen(file_name, "wb");
	if (file == NULL) {
		printf("Can't open file\n");
		return;
	}

	for (size_t i = 0; i < number_of_numbers - 1; i++)
		fprintf(file, "%d ", sort_buffer[i]);
	fprintf(file, "%d", sort_buffer[number_of_numbers - 1]);
	fclose(file);

	free(sort_buffer);
}

struct pair{
	int key;
	FILE* value;
};

int compare_pairs(const struct pair *a, const struct pair *b) {
	return a->key - b->key;
}

int main(int argc, char* argv[])
{
	if (argc <= 1)
		return 0;

	size_t number_of_files = argc - 1;

	for (size_t i = 0; i < number_of_files; i++)
		sort_file(argv[i + 1]);

	/**
	* Merge sorted files into "result.txt".
	*/
	pair *merge_buf = (pair*)malloc(sizeof(pair) * argc);
	FILE *file;
	int buf = 0;
	size_t already_read_file_counter = 0;
	for (size_t i = 0; i < number_of_files; i++) {
		file = fopen(argv[i + 1], "r");
		if (fscanf(file, "%d", &buf) != EOF) {
			merge_buf[i].key = buf;
			merge_buf[i].value = file;
		} else
			already_read_file_counter++;
	}

	qsort(merge_buf, number_of_files - already_read_file_counter, sizeof(struct pair), (int(*) (const void*, const void*)) compare_pairs);

	FILE* result = fopen("result.txt", "wb");
	while (already_read_file_counter < number_of_files) {
		fprintf(result, "%d ", merge_buf[already_read_file_counter].key);
		if (fscanf(merge_buf[already_read_file_counter].value, "%d", &merge_buf[already_read_file_counter].key) != EOF) {
			qsort(merge_buf + sizeof(pair) * already_read_file_counter, argc - 1 - already_read_file_counter, sizeof(struct pair), (int(*) (const void*, const void*)) compare_pairs);
		} else
			already_read_file_counter++;
	}

	/**
	* Add last file, if any.
	*/
	if (number_of_files - already_read_file_counter)
		while (fscanf(merge_buf[already_read_file_counter].value, "%d", &buf) != EOF)
			fprintf(result, "%d ", buf);

	free(merge_buf);
	fclose(result);
}
