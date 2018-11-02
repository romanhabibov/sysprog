#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
	char *_input_file;
	char *_output_file;
	struct simple_cmd *_simple_cmds;
	int _background;
};

char*
get_str(char* line) {
	size_t size = 64;
	char* res = malloc(sizeof(char) * size);
	assert(res);

	size_t i = 0;
	while(((*line) != '\0') ||
		  ((*line) != ' ')) {
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
	while ((c != '\n') || (c != EOF)) {
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
get_simple_cmd(char *line) {
	struct simple_cmd *res;
	size_t args_size = 64;
	res = alloc_simple_cmd(args_size);
	assert(res);

	size_t i = 0;
	size_t num_of_args = 0;
	while ((line[i] != '|') ||
		   (line[i] != '<') ||
		   (line[i] != '>') ||
		   (line[i] != '\0')) {
		size_t arg_i = 0;
		size_t arg_size = 64;
		char *arg = malloc(sizeof(char) * arg_size);
		assert(arg);
		while (line[i] != ' ') {
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
get_full_cmd(char *line) {
	struct full_cmd *res = malloc(sizeof(struct full_cmd));
	assert(res);

	size_t simple_cmds_size = 4;
	struct simple_cmd *simple_cmds = malloc(sizeof(struct simple_cmd) * simple_cmds_size);
	assert(simple_cmds);

	size_t simple_cmd_i = 0;
	do {
		simple_cmds[simple_cmd_i] = get_simple_cmd(line);
		simple_cmd_i++;
		if (simple_cmd_i == simple_cmds_size) {
			simple_cmds_size += 4;
			simple_cmds = realloc(simple_cmds, sizeof(struct simple_cmd) * simple_cmds_size);
			assert(simple_cmds);
		}
		if ((*line) == '|') {
			line += 2;
			continue;
		}
	} while (((*line) != '\0') ||
			 ((*line) != '<') ||
			 ((*line) != '>'));
	simple_cmds[simple_cmd_i] = NULL;
	res->_simple_cmds = simple_cmds;
	res->_num_of_simple_cmds = simple_cmd_i;

	if ((*line) == '>') {
		line++;
		if ((*line) == '>') {
			line++;
		}
		line++;

		res->_input_file = get_str(line);
	}

	if ((*line) == '<') {
		line++;
		if ((*line) == '<') {
			line++;
		}
		line++;

		res->_input_file = get_str(line);
	}

	return res;
}

int
launch(struct full_cmd *full_cmd) {
	int tmp_in = dup(0);
	int tmp_out dup(1);

	int fdin;
	int ret;
	if(full_cmd->_input_file) {
		fdin = open(full_cmd->_input_file, O_RONLY);
	} else
		fdin = dup(tmp_in);

	int fdout;
	for (size_t i = 0; i < full_cmd->_num_of_simple_cmds; i++) {
		dup2(fdin, 0);
		close(fdin);
		if ((full_cmd->_num_of_simple_cmds - 1) == i) {
			if (full_cmd->_output_file)
				fdout = open(full_cmd->_output_file, O_CREAT | O_WRONLY);
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
			execvp(full_cmd->_simple_cmds->_cmd_args[0],
				   full_cmd->_simple_cmds->_cmd_args);
			perror("execvp");
			exit(1);
		}
	}

	dup2(tmp_in, 0);
	dup2(tmp_out, 1);
	close(tmp_in);
	close(tmp_out);

	if (!full_cmd->_background)
		waitpid(ret, NULL);

	return 1;
}

size_t number_of_builtins = 3;

int
cd(char **args) {
	if (args[1] == NULL) {
		args[1] = "~";
	} else
		if (chdir(args[1])) {
			perror("Error");
	}
	return 1;
}

int
help(char **args)
{
	printf("Type command and press \"Enter\".\n");
	printf("Builtins:\n");

	for (size_t i = 0; i < number_of_builtins; i++) {
		printf("  %s\n", builtin_str[i]);
	}
	printf("Type \"man\" for another info.\n");
	return 1;
}

int
exit(char **args)
{
  return 0;
}

char *builtin_str[] = {
	"cd",
	"help",
	"exit",
	NULL
};

int (*builtin_func[]) (char **) = {
	&cd,
	&help,
	&exit,
	NULL
};

int
execute(char **args)
{
	if (args[0] == NULL)
		return 1;

	for (size_t i = 0; i < number_of_builtins; i++)
		if (!strcmp(args[0], builtin_str[i]))
			return (*builtin_func[i])(args);

	return launch(args);
}

int
main() {
	char *line = NULL;
	char **args;
	int status;

	do {
		while (*line != '\0')
		printf("> ");
		line = read_line();
		args = get_command(line);
		if (*line == '<') {
			line++;
			bool flag = false;
			if (*(line) == '<') {
				line++;
				flag = true;
			}
			line++;

			size_t i = 0;
			while (line[i] != ' ')
				i++;
			line[i] = '\0';

			int file;
			if (!flag) {
				file = open(line, O_RDONLY);
				dup2(file, 0);
			} else {

			}
		} else if (*line == '>') {
			line++;
			bool flag = false;
			if (*(line) == '>') {
				line++;
				flag = true;
			}
			line++;

			size_t i = 0;
			while (line[i] != ' ')
				i++;
			line[i] = '\0';

			int file;
			if (!flag) {
				file = open(line, O_CREAT | O_WRONLY);
				dup2(file, 1);
			} else {
				file = open(line, O_CREAT | O_WRONLY | O_APPEND);
				dup2(file, 1);
			}
		}

		status = execute(args);

		free(line);
		free(args);
	} while (status);
}
