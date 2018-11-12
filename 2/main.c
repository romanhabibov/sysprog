#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdbool.h>
#include "shell.h"

size_t number_of_builtins = 3;

char *line = NULL;

struct simple_cmd {
	size_t _num_of_args;
	char **_cmd_args;
};

struct simple_cmd *
alloc_simple_cmd(size_t length)
{
	struct simple_cmd *ret = malloc(sizeof(struct simple_cmd));
	ret->_cmd_args = malloc(sizeof(char *) * length);
	return ret;
}

struct simple_cmd *
realloc_simple_cmd(struct simple_cmd *cmd, size_t length)
{
	cmd->_cmd_args = realloc(cmd->_cmd_args, sizeof(char *) * length);
	return cmd;
}

struct full_cmd {
	size_t _num_of_simple_cmds;
	bool _input_flag;
	bool _output_flag;
	int _background;
	char *_input_file;
	char *_output_file;
	struct simple_cmd *_simple_cmds;
};

void free_simple_cmd(struct simple_cmd *simple_cmd) {
	for (size_t i = 0; i < simple_cmd->_num_of_args; i++)
		free(*(simple_cmd->_cmd_args) + i);
	free(simple_cmd->_cmd_args);
	return;
}

void free_full_cmd(struct full_cmd *full_cmd) {
	free(full_cmd->_input_file);
	free(full_cmd->_output_file);
	for (size_t i = 0; i < full_cmd->_num_of_simple_cmds; i++)
		free_simple_cmd(full_cmd->_simple_cmds + i);
	return;
}

char*
get_str() {
	size_t size = 64;
	char* res = malloc(sizeof(char) * size);
	assert(res);

	size_t i = 0;
	while(((*line) != '\0') && ((*line) != ' ')) {
		res[i] = *line;
		i++;
		line++;
		if (i == size) {
			size += 64;
			res = realloc(res, sizeof(char) * size);
			assert(res);
		}
	}
	res[i] = '\0';
	line++;

	return res;
}

char *
read_line() {
	size_t bufsize = 1024;
	char *str_buf = malloc(sizeof(char) * bufsize);
	assert(str_buf);

	int c = getchar();
	size_t i = 0;
	while ((c != '\n') && (c != EOF)) {
		str_buf[i] = c;
		c = getchar();
		i++;

		if (i == bufsize) {
			bufsize += 1024;
			str_buf = realloc(str_buf, sizeof(char) * bufsize);
			assert(str_buf);
		}
	}
	str_buf[i] = '\0';
	return str_buf;
}

struct simple_cmd *
get_simple_cmd() {
	struct simple_cmd *res;
	size_t args_size = 64;
	res = alloc_simple_cmd(args_size);
	assert(res);

	size_t i = 0;
	size_t num_of_args = 0;
	while ((line[i] != '|') && (line[i] != '<') &&
		   (line[i] != '>') && (line[i] != '\0')) {
		size_t arg_i = 0;
		size_t arg_size = 64;
		char *arg = malloc(sizeof(char) * arg_size);
		assert(arg);
		char stop_symbol = ' ';
		if (line[i] == '"') {
			stop_symbol = '"';
			i++;
		}
		while ((line[i] != stop_symbol) && (line[i] != '\0')) {
			if (line[i] == '\\') {
				i++;
				if (line[i] == '"') {
					arg[arg_i] = '"';
					i++;
				} else
					arg[arg_i] = '\\';
				arg_i++;
				continue;
			}
			arg[arg_i] = line[i];
			arg_i++;
			i++;

			if (arg_i == arg_size) {
				arg_size += 64;
				arg = realloc(arg, sizeof(char) * arg_size);
				assert(arg);
			}
		}
		arg[arg_i] = '\0';

		num_of_args++;
		if (args_size == num_of_args) {
			args_size += 64;
			res = realloc_simple_cmd(res, args_size);
			assert(res);
		}
		res->_cmd_args[num_of_args - 1] = arg;
		i++;
	}
	res->_cmd_args[num_of_args] = NULL;
	res->_num_of_args = num_of_args;

	line += i;

	return res;
}

struct full_cmd *
get_full_cmd() {
	struct full_cmd *res = malloc(sizeof(struct full_cmd));
	assert(res);

	size_t simple_cmds_size = 4;
	struct simple_cmd *simple_cmds = \
	malloc(sizeof(struct simple_cmd) * simple_cmds_size);
	assert(simple_cmds);

	size_t simple_cmd_i = 0;
	do {
		simple_cmds[simple_cmd_i] = *get_simple_cmd(line);
		simple_cmd_i++;
		if (simple_cmd_i == simple_cmds_size) {
			simple_cmds_size += 4;
			simple_cmds = \
			realloc(simple_cmds, sizeof(struct simple_cmd) * simple_cmds_size);
			assert(simple_cmds);
		}
		if ((*line) == '|') {
			line += 2;
			continue;
		}
	} while (((*line) != '\0') &&
			 ((*line) != '<') &&
			 ((*line) != '>'));
	res->_simple_cmds = simple_cmds;
	res->_num_of_simple_cmds = simple_cmd_i;

	if ((*line) == '>') {
		line++;
		if ((*line) == '>') {
			line++;
			res->_output_flag = true;
		} else
			res->_output_flag = false;
		line++;

		res->_input_file = get_str(line);
	}

	if ((*line) == '<') {
		line++;
		if ((*line) == '<') {
			line += 2;
			char *terminator = get_str();
			int temp_doc = open("temp_doc.txt", O_CREAT | O_WRONLY);
			res->_input_file = "temp_doc.txt";
			res->_input_flag = true;
			printf("> ");
			char *temp_str = read_line();
			while (strcmp(terminator, temp_str)) {
				write(temp_doc, temp_str, sizeof(temp_str));
				write(temp_doc, "\n", 1);
				free(temp_str);
				printf("> ");
				temp_str = read_line();
			}
			write(temp_doc, EOF, 1);
			free(temp_str);
			close(temp_doc);
		} else {
			res->_input_file = get_str();
			res->_input_flag = false;
		}
		line++;
	}


	return res;
}

int
launch(struct full_cmd *full_cmd) {
	int tmp_in = dup(0);
	int tmp_out = dup(1);

	int fdin;
	int ret;
	if(full_cmd->_input_file) {
		fdin = open(full_cmd->_input_file, O_RDONLY);
	} else
		fdin = dup(tmp_in);

	int fdout;
	for (size_t i = 0; i < full_cmd->_num_of_simple_cmds; i++) {
		dup2(fdin, 0);
		close(fdin);
		if ((full_cmd->_num_of_simple_cmds - 1) == i) {
			if (full_cmd->_output_file)
				if (full_cmd->_output_flag)
					fdout = open(full_cmd->_output_file,
								 O_CREAT | O_WRONLY | O_APPEND);
				else
					fdout = open(full_cmd->_output_file,
								 O_CREAT | O_WRONLY);
			else
				fdout = dup(tmp_out);
		} else {
			int fdpipe[2];
			pipe(fdpipe);
			fdout = fdpipe[1];
			fdin = fdpipe[0];
		}

		dup2(fdout, 1);
		close(fdout);

		ret = fork();
		if (!ret) {
			execvp(full_cmd->_simple_cmds[i]._cmd_args[0],
				   full_cmd->_simple_cmds[i]._cmd_args);
			perror("execvp");
			exit(1);
		}
	}

	if (full_cmd->_input_flag)
		unlink(full_cmd->_input_file);
	dup2(tmp_in, 0);
	dup2(tmp_out, 1);
	close(tmp_in);
	close(tmp_out);

	if (!full_cmd->_background)
		waitpid(ret, NULL);

	return 1;
}

int
execute(struct full_cmd *full_cmd)
{
	if (full_cmd->_simple_cmds->_cmd_args == NULL)
		return 1;

	if (!strcmp(full_cmd->_simple_cmds->_cmd_args[0], "cd")) {
		if (!full_cmd->_simple_cmds->_cmd_args[1]) {
			full_cmd->_simple_cmds->_cmd_args[1] = "~\0";
		} else
			if (chdir(full_cmd->_simple_cmds->_cmd_args[1]))
				perror("Error");
		return 1;
	}
	else if (!strcmp(full_cmd->_simple_cmds->_cmd_args[0], "help")) {
		printf("Type command and press \"Enter\".\n");
		printf("Builtins:\n");

		for (size_t i = 0; i < number_of_builtins; i++)
			printf("  %s\n", builtin_str[i]);
		printf("Type \"man\" for another info.\n");
		return 1;
	}
	else if (!strcmp(full_cmd->_simple_cmds->_cmd_args[0], "exit"))
		return 0;

	return launch(full_cmd);
}

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
