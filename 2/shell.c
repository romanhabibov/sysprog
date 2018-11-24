#include "shell.h"

size_t number_of_builtins = 3;

char *builtin_str[] = {
	"cd",
	"help",
	"exit",
	NULL
};

char *line = NULL;
char **tokens = NULL;
int status = 0;

void
init_full_cmd(struct full_cmd *full_cmd) {
	full_cmd->_background = false;
	full_cmd->_for_next = 0;
	full_cmd->_input_file = NULL;
	full_cmd->_input_flag = false;
	full_cmd->_output_file = NULL;
	full_cmd->_output_flag = false;
	full_cmd->_num_of_simple_cmds = 0;
	full_cmd->_simple_cmds = NULL;
}

void free_full_cmd(struct full_cmd *full_cmd) {
	if (full_cmd->_simple_cmds)
		free(full_cmd->_simple_cmds);
	free(full_cmd);
	return;
}

char *
read_line() {
	size_t bufsize = 1024;
	char *str_buf = malloc(sizeof(char) * bufsize);
	assert(str_buf);

	int c = getchar();
	if (c == EOF)
		exit(1);
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

char * __attribute__ ((malloc))
get_token() {
	size_t token_i = 0;
	size_t token_size = 64;
	char *token = malloc(sizeof(char) * token_size);
	assert(token);

	char stop_symbol = ' ';
	if (((*line) == '"') || ((*line) == '\'')) {
		stop_symbol = (*line);
		line++;
	}
	while (((*line) != stop_symbol) && ((*line) != '\0')) {
		if ((*line) == '\\') {
			line++;
			if (((*line) == '"') || ((*line) == '\'') || ((*line) == ' ')) {
				if (stop_symbol != ' ') {
					token[token_i] = '\\';
					token_i++;
				}
				token[token_i] = (*line);
				line++;
			} else
				token[token_i] = '\\';
			token_i++;
			continue;
		}
		token[token_i] = (*line);
		token_i++;
		line++;

		if (token_i == token_size) {
			token_size += 64;
			token = realloc(token, sizeof(char) * token_size);
			assert(token);
		}
	}
	token[token_i] = '\0';

	if (stop_symbol != ' ')
		line++;

	if ((*line) == ' ')
		line++;

	return token;
}

size_t
get_tokens() {
	size_t tokens_size = 32;
	tokens = malloc(tokens_size * sizeof(char *));
	assert(tokens);

	size_t number_of_tokens = 0;
	while ((*line) != '\0') {
		char *token = get_token();

		number_of_tokens++;
		if (tokens_size == number_of_tokens) {
			tokens_size += 32;
			tokens = realloc(tokens, sizeof(char *) * tokens_size);
			assert(tokens);
		}
		tokens[number_of_tokens - 1] = token;
	}
	tokens[number_of_tokens] = NULL;

	return number_of_tokens;
}

struct full_cmd * __attribute__ ((malloc))
get_full_cmds(size_t *number_of_full_cmds) {
	size_t full_cmds_size = 4;
	struct full_cmd *full_cmds = malloc(full_cmds_size * sizeof(struct full_cmd));
	assert(full_cmds);

	*number_of_full_cmds = 0;
	while ((*tokens) != NULL) {
		init_full_cmd(full_cmds + *number_of_full_cmds);
		full_cmds[*number_of_full_cmds]._simple_cmds = \
		get_simple_cmds(&full_cmds[*number_of_full_cmds]._num_of_simple_cmds);
		while ((*tokens) != NULL) {
			if (!strcmp((*tokens), "&")) {
				*tokens = NULL;
				tokens++;
				full_cmds[*number_of_full_cmds]._for_next = 0;
				full_cmds[*number_of_full_cmds]._background = true;
				break;
			}
			if (!strcmp((*tokens), "&&")) {
				*tokens = NULL;
				tokens++;
				full_cmds[*number_of_full_cmds]._for_next = 1;
				break;
			}
			if (!strcmp((*tokens), "||")) {
				*tokens = NULL;
				tokens++;
				full_cmds[*number_of_full_cmds]._for_next = 2;
				break;
			}
			if (!strcmp((*tokens), ">")) {
				*tokens = NULL;
				tokens++;
				if ((*tokens) == NULL)
					break;
				full_cmds[*number_of_full_cmds]._output_file = (*tokens);
				full_cmds[*number_of_full_cmds]._output_flag = false;
				tokens++;
			} else if (!strcmp((*tokens), ">>")) {
				*tokens = NULL;
				tokens++;
				if ((*tokens) == NULL)
					break;
				full_cmds[*number_of_full_cmds]._output_file = (*tokens);
				full_cmds[*number_of_full_cmds]._output_flag = true;
				tokens++;
			} else if (!strcmp((*tokens), "<")) {
				*tokens = NULL;
				tokens++;
				if ((*tokens) == NULL)
					break;
				full_cmds[*number_of_full_cmds]._input_file = (*tokens);
				full_cmds[*number_of_full_cmds]._input_flag = false;
				tokens++;
			} else if (!strcmp((*tokens), "<<")) {
				*tokens = NULL;
				tokens++;
				if ((*tokens) == NULL)
					break;
				char *terminator = (*tokens);
				int temp_doc = open("temp_doc.txt", O_CREAT | O_WRONLY, 00700);
				full_cmds[*number_of_full_cmds]._input_file = "temp_doc";
				full_cmds[*number_of_full_cmds]._input_flag = true;
				printf("> ");
				char *temp_str = read_line();
				while (strcmp(terminator, temp_str)) {
					write(temp_doc, temp_str, sizeof(temp_str));
					write(temp_doc, "\n", 1);
					free(temp_str);
					printf("> ");
					temp_str = read_line();
				}
				free(temp_str);
				close(temp_doc);
				tokens++;
			}
			else
				break;
		}
		(*number_of_full_cmds)++;
		if (*number_of_full_cmds == full_cmds_size) {
			full_cmds_size += 4;
			full_cmds = realloc(full_cmds, sizeof(char) * full_cmds_size);
			assert(full_cmds);
		}
	}

	return full_cmds;
}

struct simple_cmd * __attribute__ ((malloc))
get_simple_cmds(size_t *number_of_simple_cmds) {
	size_t simple_cmds_size = 4;
	struct simple_cmd *simple_cmds = malloc(simple_cmds_size * sizeof(struct simple_cmd));
	assert(simple_cmds);

	size_t i = 0;
	while (tokens[i] != NULL) {
		if (!(strcmp(tokens[i], ">") &&
			  strcmp(tokens[i], "<") &&
			  strcmp(tokens[i], ">>") &&
			  strcmp(tokens[i], "<<") &&
			  strcmp(tokens[i], "&") &&
			  strcmp(tokens[i], "&&") &&
			  strcmp(tokens[i], "||")))
			break;
		if (!strcmp(tokens[i], "|")) {
			simple_cmds[*number_of_simple_cmds]._num_of_args = i;
			simple_cmds[*number_of_simple_cmds]._cmd_args = tokens;
			(*number_of_simple_cmds)++;
			tokens[i] = NULL;
			tokens += i + 1;
			i = 0;
		} else
			i++;

		if (*number_of_simple_cmds == simple_cmds_size) {
			simple_cmds_size += 4;
			simple_cmds = \
			realloc(simple_cmds, sizeof(struct simple_cmd) * simple_cmds_size);
			assert(simple_cmds);
		}
	}
	simple_cmds[*number_of_simple_cmds]._num_of_args = i;
	simple_cmds[*number_of_simple_cmds]._cmd_args = tokens;
	(*number_of_simple_cmds)++;
	tokens += i;

	return simple_cmds;
}

int
launch(struct full_cmd *full_cmd) {
	int tmp_in = dup(0);
	int tmp_out = dup(1);

	int fdin;
	int ret;
	if(full_cmd->_input_file) {
		fdin = open(full_cmd->_input_file, O_RDONLY, 00700);
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
								 O_CREAT | O_WRONLY | O_APPEND, 00700);
				else
					fdout = open(full_cmd->_output_file,
								 O_CREAT | O_WRONLY | O_TRUNC, 00700);
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

		switch (ret = fork()) {
		case -1 :
			status = -1;
			perror("fork");
			break;
		case 0 :
			status = execvp(full_cmd->_simple_cmds[i]._cmd_args[0],
							full_cmd->_simple_cmds[i]._cmd_args);
			perror("execvp");
			exit(status);
		}
	}

	if (full_cmd->_input_flag)
		unlink(full_cmd->_input_file);
	dup2(tmp_in, 0);
	dup2(tmp_out, 1);
	close(tmp_in);
	close(tmp_out);

	if (!full_cmd->_background)
		waitpid(ret, &status, 0);

	return status;
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
		exit(1);

	return launch(full_cmd);
}
