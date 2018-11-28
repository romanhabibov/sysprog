#include "shell.h"

extern char *line;
extern char **tokens;
extern int status;

int
main() {
	struct full_cmd* full_cmds = NULL;

	while (true) {
		printf("$ ");
		line = read_line();
		char *pointer0 = line;
		char **tokenz;
		size_t n = get_tokens(&tokenz);
		if (!n)
			continue;
		char **pointer1 = tokens;
		size_t number_of_full_cmds;
		full_cmds = get_full_cmds(&number_of_full_cmds);
		int for_next;
		for (size_t i = 0; i < number_of_full_cmds; i++) {
			for_next = full_cmds[i]._for_next;
			status = execute(full_cmds + i);
			if (for_next == 1) {
				if (status) {
					do
						i++;
					while (full_cmds[i]._for_next == 1);
					continue;
				}
			} else if (for_next == 2)
				if (!status) {
					do
						i++;
					while (full_cmds[i]._for_next == 2);
					continue;
				}
		}
		free(pointer0);
		for(size_t i = 0; i < n; i++)
			free(pointer1[i]);
		free(pointer1);
		free_full_cmd(full_cmds);
	}

	return 0;
}
