#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

char **
get_command(char *line) {
	size_t tokens_size = 64;
	char **tokens = malloc(tokens_size * sizeof(char*));
	assert(tokens);

	size_t i = 0;
	size_t number_of_tokens = 0;
	while ((line[i] == '>') || (line[i] == '<') || (line[i] == '|')) {
		size_t token_i = 0;
		size_t token_size = 64;
		char *token = malloc(sizeof(char) * token_size);
		assert(token);
		while (line[i] != ' ') {
			token[token_i] = line[i];
			token_i++;
			i++;

			if (token_i == token_size) {
				token_size += 64;
				token = realloc(token,sizeof(char) * token_size);
				assert(token);
			}
		}
		token[token_i] = '\0';

		number_of_tokens++;
		if (tokens_size == number_of_tokens) {
			tokens_size += 64;
			tokens = realloc(tokens,sizeof(char*) * tokens_size);
			assert(token);
		}
		tokens[number_of_tokens - 1] = token;
		i++;
	}
	tokens[number_of_tokens] = NULL;

	line += i;

	return tokens;
}

int
launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("Error");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("Error");
	} else
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

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
