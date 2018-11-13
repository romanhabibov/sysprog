#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

struct simple_cmd {
	size_t _num_of_args;
	char **_cmd_args;
};

struct full_cmd {
	size_t _num_of_simple_cmds;
	bool _input_flag;
	bool _output_flag;
	bool _background;
	char *_input_file;
	char *_output_file;
	struct simple_cmd *_simple_cmds;
};

void
free_simple_cmd(struct simple_cmd *simple_cmd);

void
free_full_cmd(struct full_cmd *full_cmd);

char*
get_str();

char *
read_line();

void
get_simple_cmd(struct simple_cmd *simple_cmd);

struct full_cmd *
get_full_cmd();

int
launch(struct full_cmd *full_cmd);

int
execute(struct full_cmd *full_cmd);
