#include "pipex.h"
/******************************************************************************************/
void	clear_str_array(char **str)
{
	int	i;

	i = 0;
	while (str[i])
		free(str[i++]);
	free(str);
}

void	apnd_cmd(char *cmd, char **path)
{
	int		i;
	char	*slashed;
	char	*withcmd;

	i = 0;
	while (path[i])
	{
		slashed = ft_strjoin(path[i], "/");
		free(path[i]);
		path[i] = slashed;
		withcmd = ft_strjoin(path[i], cmd);
		free(path[i]);
		path[i] = withcmd;
		i++;
	}
}

char	**bin_path(char **envp)
{
	int		i;
	int		j;
	char	**path;
	char	*tmp;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			break ;
		i++;
	}
	j = 0;
	while (envp[i][j] && envp[i][j] != '=')
		j++;
	j++;
	path = ft_split(&(envp[i][j]), ':');
	return (path);
}

char	*find_cmd_path(char **argv, char **envp)
{
	char	**path;
	char	*result;
	int		i;

	path = bin_path(envp);
	apnd_cmd(argv[0], path);
	i = 0;
	while (path[i])
	{
		if (access(path[i], X_OK) == 0)
			break ;
		i++;
	}
	result = 0;
	if (path[i] == 0)
		perror(argv[0]);
	else
		result = ft_strdup(path[i]);
	clear_str_array(path);

	return (result);
}
/******************************************************************************************/
void	exec_child(char *cmd, int infd, int outfd, char **envp)
{
	pid_t	cpid;
	char	*cmd_path;
	char	**argv;
	char	buf;

	argv = ft_split(cmd, ' ');
	if (!argv)
		return ;
	cmd_path = find_cmd_path(argv, envp);
	if (!cmd_path)
		return ;
	cpid = fork();
	if (cpid == 0)
	{
		dup2(infd, 0);
		dup2(outfd, 1);
		execve(cmd_path, argv, envp);
	}
	clear_str_array(argv);
	free(cmd_path);
}

void	exec(char *cmd, int *inpipe, int *outpipe, char **envp)
{
	char	buf;

	close(inpipe[1]);
	exec_child(cmd, inpipe[0], outpipe[1], envp);
	close(inpipe[0]);
	pipe(inpipe);
	close(outpipe[1]);
	while (read(outpipe[0], &buf, 1))
		write(inpipe[1], &buf, 1);
	close(outpipe[0]);
	pipe(outpipe);
}

int	main(int argc, char **argv, char **envp)
{
	int		infd;
	int		inpipe[2];
	int		outpipe[2];
	char	buf;
	pid_t	cpid;
	int		i;

/* 	if (argc < 5)
		return ((perror("Wrong arguments\n"), 0)); */
	if (pipe(inpipe) == -1)
		perror("pipe");
	if (pipe(outpipe) == -1)
		perror("pipe");
	infd = open(argv[1], O_RDONLY);
	if (infd == -1)
	{
		perror("weird ass file\n");
		exit(0);
	}
	while (read(infd, &buf, 1))
		write(inpipe[1], &buf, 1);
	exec("cat", inpipe, outpipe, envp);
	close(inpipe[1]);
	while (read(inpipe[0], &buf, 1))
		write(1, &buf, 1);
	return (0);
}
