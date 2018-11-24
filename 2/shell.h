#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

struct simple_cmd {
	size_t _num_of_args;
	char **_cmd_args;
};

struct full_cmd {

	size_t _num_of_simple_cmds;
	int _for_next;
	bool _input_flag;
	bool _output_flag;
	bool _background;
	char *_input_file;
	char *_output_file;
	struct simple_cmd *_simple_cmds;
};

void
init_full_cmd(struct full_cmd *full_cmd);

void free_full_cmd(struct full_cmd *full_cmd);

char *
read_line();

char * __attribute__ ((malloc))
get_token();

size_t
get_tokens();

struct full_cmd * __attribute__ ((malloc))
get_full_cmds(size_t *number_of_full_cmds);

struct simple_cmd * __attribute__ ((malloc))
get_simple_cmds(size_t *number_of_simple_cmds);

int
launch(struct full_cmd *full_cmd);

int
execute(struct full_cmd *full_cmd);
