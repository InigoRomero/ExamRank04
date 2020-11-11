#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_command{
	int type;
	char **argv;
	char **argvaux; 
}				t_command;

int ft_strlen(char *str)
{
	int i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

char *ft_strdup(char *src)
{
	char *dest;
	int i = 0;
	if (!(dest = malloc(sizeof(char *) * ft_strlen(src) + 1)))
		return (NULL);
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return (dest);
}

int show_error(char *str)
{
	if (str)
		write(2, str, (size_t)ft_strlen(str));
	return (0);
}

int fatal_error()
{
	show_error("error: fatal\n");
	exit(EXIT_FAILURE);
	return (EXIT_FAILURE);
}

int ft_parse_command(t_command *stru, int i)
{
	int x = 0, aux = i;
	while (stru->argvaux[i] && strcmp(stru->argvaux[i], "|") && strcmp(stru->argvaux[i], ";"))
		i++;
	if (!(stru->argv = (char**)malloc(sizeof(char**) * (i - aux + 1))))
		return (0);
	while (x + aux < i)
	{
		stru->argv[x] = ft_strdup(stru->argvaux[x + aux]);
		x++;
	}
	stru->argv[x] = 0;
	if (stru->argvaux[i] && !strcmp(stru->argvaux[i], ";"))
		stru->type = 1;
	if (stru->argvaux[i] && !strcmp(stru->argvaux[i], "|"))
		stru->type = 2;
	return (i);
}

void ft_cd(char **argv)
{
	int i = 0;
	while(argv[i])
		i++;
	if (i < 1)
		show_error("error: cd: bad arguments\n");
	else if (chdir(argv[1]))
	{
		show_error("error: cd: cannot change directory to ");
		show_error(argv[1]);
		show_error("\n");
	}
}

int main(int argc, char **argv, char **env)
{
	int i = 1, pipes[2], status;
	pid_t pid;
	t_command stru;

	stru.argvaux = argv;
	while (i < argc)
	{
		stru.type = 0;
		i = ft_parse_command(&stru, i);
		if (stru.type == 2 && pipe(pipes))
			return (fatal_error());
		pid = fork();
		if (pid == 0)
		{
			if (stru.type == 2)
				dup2(pipes[1], 1);
			if (!strcmp(stru.argv[0], "cd"))
				ft_cd(stru.argv);
			else if ((execve(stru.argv[0], stru.argv, env)) < 0)
			{
				show_error("error: cannot execute ");
				show_error(stru.argv[0]);
				show_error("\n");
			}
		}
		else
			waitpid(pid, &status, 0);
		if (stru.type == 2)
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