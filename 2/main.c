#include "shell.h"

extern char *line;

int
main() {
	struct full_cmd* full_cmd = NULL;
	int status;

	do {
		printf("$ ");
		line = read_line();
		char *pointer = line;
		full_cmd = get_full_cmd(line);
		status = execute(full_cmd);

		free(pointer);
		free_full_cmd(full_cmd);
	} while (status);

return 0;
}
