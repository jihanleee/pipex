/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihalee <jihalee@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 16:28:41 by jihalee           #+#    #+#             */
/*   Updated: 2023/08/07 16:50:28 by jihalee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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
	if (path == 0)
	{
		ft_putstr_fd("malloc error\n", 2);
		exit(EXIT_FAILURE);
	}
	return (path);
}

char	*find_cmd_path(char *cmd, char **envp)
{
	int		i;
	char	*slashed;
	char	*withcmd;
	char	**path;

	path = bin_path(envp);
	i = 0;
	while (path[i])
	{
		slashed = ft_strjoin(path[i], "/");
		withcmd = ft_strjoin(slashed, cmd);
		if (withcmd == 0)
			return (NULL);
		if (access(withcmd, X_OK) == 0)
			break ;
		i++;
	}
	if (path[i] == 0)
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(EXIT_FAILURE);
	}
	return (withcmd);
}

void	exec_child(char *cmd, int infd, int outfd, char **envp)
{
	pid_t	cpid;
	char	*cmd_path;
	char	**argv;
	char	buf;

	cpid = fork();
	if (cpid == 0)
	{
		argv = ft_split(cmd, ' ');
		if (!argv)
		{
			ft_putstr_fd("malloc error\n", 2);
			exit(EXIT_FAILURE);
		}
		cmd_path = find_cmd_path(argv[0], envp);
		if (!cmd_path)
		{
			ft_putstr_fd("malloc error\n", 2);
			exit(EXIT_FAILURE);
		}
		dup2(infd, 0);
		dup2(outfd, 1);
		execve(cmd_path, argv, envp);
	}
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
	wait(0);
}

int	main(int argc, char **argv, char **envp)
{
	int		infd;
	int		outfd;
	int		inpipe[2];
	int		outpipe[2];
	char	buf;

	if (argc != 5)
		return ((ft_putstr_fd("Wrong arguments\n", 2), EXIT_FAILURE));
	if (pipe(inpipe) == -1 || pipe(outpipe) == -1)
		return ((perror("pipe"), EXIT_FAILURE));
	outfd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	infd = open(argv[1], O_RDONLY);
	if (outfd == -1 || infd == -1)
		return ((perror("file"), EXIT_FAILURE));
	while (read(infd, &buf, 1))
		write(inpipe[1], &buf, 1);
	exec(argv[2], inpipe, outpipe, envp);
	exec(argv[3], inpipe, outpipe, envp);
	close(inpipe[1]);
	while (read(inpipe[0], &buf, 1))
		write(outfd, &buf, 1);
	return (0);
}
