
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct	s_command
{
	char			**argv;
	char			**argvAux;
	int				type;
}				t_command;

int ft_strlen(char *str)
{
	int i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

char	*ft_strdup(char *str)
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

int show_error(char *str)
{
	if (str)
		write(2, str, ft_strlen(str));
	return (0);
}

int exit_fatal(void)
{
	show_error("error: fatal\n");
	exit(EXIT_FAILURE);
	return (EXIT_FAILURE);
}

int ft_cd(char **argv)
{
	int ret = 0;
	int length = 0;
	while(argv[length])
		length++;
	if (length < 1)
		ret = show_error("error: cd: bad arguments\n");
	else if (chdir(argv[1]))
	{
		ret = show_error("error: cd: cannot change directory to ");
		show_error(argv[1]);
		show_error("\n");
	}
	return (ret);
}

int	parse_command(t_command *stru, int i)
{
	int aux = i, x = 0;

	while (stru->argvAux[i] && strcmp(";", stru->argvAux[i]) != 0 && strcmp("|", stru->argvAux[i]) != 0)
		i++;
	if (!(stru->argv = (char**)malloc(sizeof(char**) * (i - aux + 1))))
		return -1;
	while (aux + x < i){
		stru->argv[x] = ft_strdup(stru->argvAux[aux + x]);
		x++;
	}
	stru->argv[x] = 0;
	if (stru->argvAux[i] && strcmp(";", stru->argvAux[i]) == 0)
	{
		stru->type = 1;
		i++;
	}
	if (stru->argvAux[i] && strcmp( "|", stru->argvAux[i]) == 0)
	{
		stru->type = 2;
		i++;
	}
	return (i);
}

int main(int argc, char **argv, char **env)
{
	int ret;
	int status;
	int i = 1;
	int				pipes[2];
	pid_t	pid;
	t_command stru;
	int		pipe_open;

	stru.argvAux = argv;
	while (i < argc){
		pipe_open = 0;
		stru.type = 0;
		i = parse_command(&stru, i);
		if (stru.type == 2)
		{
			pipe_open = 1;
			if (pipe(pipes))
				return (exit_fatal());
		}
		pid = fork();
		if (pid < 0)
			exit_fatal();
		if (pid == 0)
		{
			
			if (pipe_open)
				dup2(pipes[1], 1);
			if (strcmp("cd", stru.argv[0]) == 0)
				ft_cd(stru.argv);
			else if ((ret = execve(stru.argv[0], stru.argv, env)) < 0)
			{
				show_error("error: cannot execute ");
				show_error(stru.argv[0]);
				show_error("\n");
			}
			exit(ret);
		}else{
			waitpid(pid, &status, 0);
			if (pipe_open)
			{
				dup2(pipes[0], 0);
				close(pipes[0]);
				close(pipes[1]);
		}
	}
	int x = 0;
	while (stru.argv[x])
		free(stru.argv[x++]);
	free(stru.argv);
	if (stru.type == 0)
		break ;
	}
}