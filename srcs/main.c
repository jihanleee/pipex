/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihalee <jihalee@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 16:28:41 by jihalee           #+#    #+#             */
/*   Updated: 2023/08/07 19:26:42 by jihalee          ###   ########.fr       */
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

void	exec_child1(char *cmd, int infd, int pipefd[2], char **envp)
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
		close(pipefd[0]);
		if (infd != 1)
			dup2(pipefd[1], 1);
		execve(cmd_path, argv, envp);
	}
	close(pipefd[1]);
}

void	exec_child2(char *cmd, int outfd, int pipefd[2], char **envp)
{
	pid_t	cpid;
	char	*cmd_path;
	char	**argv;
	char	buf;

	cpid = fork();//protect fork
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
		/* close(pipefd[1]); */
		dup2(pipefd[0], 0);
		if (outfd != 1)
			dup2(outfd, 1);
		execve(cmd_path, argv, envp);
	}
	close(pipefd[0]);
}


int	main(int argc, char **argv, char **envp)
{
	int		infd;
	int		outfd;
	int		pipefd[2];
	char	buf;
	int		wstatus;

	if (argc != 5)
		return ((ft_putstr_fd("Wrong arguments\n", 2), EXIT_FAILURE));
	if (pipe(pipefd) == -1)
		return ((perror("pipe"), EXIT_FAILURE));
	outfd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	infd = open(argv[1], O_RDWR);
	if (outfd == -1 || infd == -1)
		return ((perror("file"), EXIT_FAILURE));
	exec_child1(argv[2], infd, pipefd, envp);
	exec_child2(argv[3], outfd, pipefd, envp);
	close(infd);
	wait(0);
	wait(0);
	return (0);
}
