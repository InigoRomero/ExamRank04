#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct	s_command{
	int type;
	char **argv;
	char **argvaux;
}				t_command;

int ft_strlen(char *src)
{
	int i = 0;
	while (src[i])
		i++;
	return (i);
}

char *ft_strdup(char *str)
{
	int i = 0;
	char *dest;
	if (!(dest = malloc(ft_strlen(str) + 1)))
		return (NULL);
	while (str[i] != '\0')
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

int show_error(char *src)
{
	if (src)
		write (2, src, (size_t)ft_strlen(src));
	return (0);
}

int exit_fatal()
{
	show_error("error: fatal\n");
	exit(EXIT_FAILURE);
	return(EXIT_FAILURE);
}

int parse_command(t_command *stru, int i)
{
	int x = 0, aux = i;
	while (stru->argvaux[i] && strcmp(stru->argvaux[i], ";") && strcmp(stru->argvaux[i], "|"))
		i++;
	if (!(stru->argv = (char**)malloc(sizeof(char**) * (i - aux + 1))))
		return (-1);
	while (x + aux < i)
	{
		stru->argv[x] = ft_strdup(stru->argvaux[x + aux]);
		x++;
	}
	stru->argv[x] = 0;
	if (stru->argvaux[i] && strcmp(";", stru->argvaux[i]) == 0)
		stru->type = 1;
	if (stru->argvaux[i] && strcmp( "|", stru->argvaux[i]) == 0)
		stru->type = 2;
	return (i);
}

int ft_cd (char **argv)
{
	int ret = 0, lenght = 0;
	while (argv[lenght])
		lenght++;
	if (lenght < 1)
		ret = show_error("error: cd: bad arguments\n");
	else if (chdir(argv[1]))
	{
		ret = show_error("error: cd: cannot change directory to ");
		show_error(argv[1]);
		show_error("\n");
	}
	return (ret);
}

int main (int argc, char **argv, char **env)
{
	int pipes[2], pipe_open, status, i = 1;
	pid_t pid;
	t_command stru;

	stru.argvaux = argv;
	while (i < argc)
	{
		stru.type = 0;
		pipe_open = 0;
		i = parse_command(&stru, i);
		if (stru.type == 2)
		{
			pipe_open = 1;
			if (pipe(pipes))
				return (exit_fatal());
		}
		pid = fork();
		if (pid < 0)
			return (exit_fatal());
		if (pid == 0)
		{
			if (pipe_open)
				dup2(pipes[1], 1);
			if (strcmp(stru.argv[0], "cd") == 0)
				ft_cd(stru.argv);
			else if(execve(stru.argv[0], stru.argv, env) < 0)
			{
				show_error("error: cannot execute ");
				show_error(stru.argv[0]);
				show_error("\n");
			}
			exit(0);
		}
		else
			waitpid(pid, &status, 0);
		if (pipe_open == 1)
		{
			dup2(pipes[0], 0);
			close(pipes[1]);
			close(pipes[0]);
		}
		int x = 0;
		while (stru.argv[x])
			free(stru.argv[x++]);
		free(stru.argv);
		if (stru.type == 0)
			break ;
		i++;
	}
}
