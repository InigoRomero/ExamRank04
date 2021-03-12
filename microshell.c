#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_command{
	char **argvaux;
	char **argv;
	int type;
}				t_command;

int ft_strlen(char *str)
{
	int i = 0;
	while (str[i])
		i++;
	return (i);
}

char *ft_strdup(char *src)
{
	char *dest;
	int i = 0;
	if (!(dest = malloc(sizeof(char*) * ft_strlen(src) + 1)))
		return (NULL);
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

int ft_show_error(char *str)
{
	if (str)
		write(2, str, (size_t)ft_strlen(str));
	return (0);
}

int fatal_error()
{
	ft_show_error("fatal: error\n");
	exit(EXIT_FAILURE);
	return (EXIT_FAILURE);
}

void	ft_cd(char **argv)
{
	int i = 0;
	while (argv[i])
		i++;
	if (i < 2)
		ft_show_error("error: cd: bad arguments\n");
	else if (chdir(argv[1]))
	{
		ft_show_error("error: cd: cannot change directory to ");
		ft_show_error(argv[1]);
		ft_show_error("\n");
	}
}

int ft_parse_command(t_command *stru, int i)
{
	int x = 0, aux = i;
	while (stru->argvaux[i] && strcmp(stru->argvaux[i], ";") && strcmp(stru->argvaux[i], "|"))
		i++;
	if (!(stru->argv = (char**)malloc(sizeof(char**) * (i - aux + 1))))
		return 0;
	while (x + aux < i)
	{
		stru->argv[x] = ft_strdup(stru->argvaux[aux + x]);
		x++;
	}
	stru->argv[x] = 0;
	if (stru->argvaux[i] && !strcmp(stru->argvaux[i], ";"))
		stru->type = 1;
	if (stru->argvaux[i] && !strcmp(stru->argvaux[i], "|"))
		stru->type = 2;
	return (i);
}

int main(int argc, char **argv, char **env)
{
	int i = 1, status, x = 0, pipes[2];
	t_command stru;
	pid_t pid;

	stru.argvaux = argv;
	while (i < argc)
	{
		x = 0;
		stru.type = 0;
		if ((i = ft_parse_command(&stru, i)) == 0)
			return (fatal_error());
		if (stru.type == 2 && pipe(pipes))
			return (fatal_error());
		pid = fork();
		if (pid < 0)
            fatal_error();
		if (pid == 0)
		{
			if (stru.type == 2)
				dup2(pipes[1], 1);
			if (!strcmp(stru.argv[0], "cd"))
				ft_cd(stru.argv);
			else if((execve(stru.argv[0], stru.argv, env)) < 0)
			{
				ft_show_error("error: cannot execute ");
				ft_show_error(stru.argv[0]);
				ft_show_error("\n");
			}
			exit(0);
		}
		else
			waitpid(pid, &status, 0);
		if (stru.type == 2)
		{
			dup2(pipes[0], 0);
			close(pipes[1]);
			close(pipes[0]);
		}
		while (stru.argv[x])
			free(stru.argv[x++]);
		free(stru.argv);
		if (stru.type == 0)
			break ;
		i++;
	}
}
