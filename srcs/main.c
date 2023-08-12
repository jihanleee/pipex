/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihalee <jihalee@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/07 16:28:41 by jihalee           #+#    #+#             */
/*   Updated: 2023/08/10 18:07:09 by jihalee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	**bin_path(char **argv, char **envp)
{
	int		i;
	int		j;
	char	**path;

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
		free_arrays(argv);
		error_exit("malloc error\n");
	}
	return (path);
}

char	*find_cmd_path(char **argv, char **envp)
{
	int		i;
	char	*slashed;
	char	*withcmd;
	char	**path;

	path = bin_path(argv, envp);
	i = 0;
	while (path[i])
	{
		slashed = ft_strjoin(path[i], "/");
		withcmd = ft_strjoin(slashed, argv[0]);
		if (slashed)
			free(slashed);
		if (withcmd == 0)
			return (free_arrays(path), NULL);
		if (access(withcmd, X_OK) == 0)
			break ;
		free(withcmd);
		withcmd = 0;
		i++;
	}
	free_arrays(path);
	if (!withcmd)
		error_exit((free_arrays(argv), "command not found\n"));
	return (withcmd);
}

void	exec_child1(char *cmd, int infd, int pipefd[2], char **envp)
{
	pid_t	cpid;
	char	*cmd_path;
	char	**argv;

	cpid = fork();
	if (cpid < 0)
		error_exit("fork error\n");
	if (cpid == 0)
	{
		close(pipefd[0]);
		if (dup2(pipefd[1], 1) < 0 || dup2(infd, 0) < 0)
			error_exit("dup2 error\n");
		argv = ft_split(cmd, ' ');
		if (!argv)
			error_exit("malloc error\n");
		cmd_path = find_cmd_path(argv, envp);
		if (!cmd_path)
			error_exit((free_arrays(argv), "malloc error\n)"));
		execve(cmd_path, argv, envp);
		free_arrays(argv);
		free(cmd_path);
		error_exit("execve error\n");
	}
	close(pipefd[1]);
}

void	exec_child2(char *cmd, int outfd, int pipefd[2], char **envp)
{
	pid_t	cpid;
	char	*cmd_path;
	char	**argv;

	cpid = fork();
	if (cpid < 0)
		error_exit("fork error\n");
	if (cpid == 0)
	{
		if (dup2(pipefd[0], 0) < 0 || dup2(outfd, 1) < 0)
			error_exit("dup2 error\n");
		argv = ft_split(cmd, ' ');
		if (!argv)
			error_exit("malloc error\n");
		cmd_path = find_cmd_path(argv, envp);
		if (!cmd_path)
			error_exit((free_arrays(argv), "malloc error\n)"));
		execve(cmd_path, argv, envp);
		free_arrays(argv);
		free(cmd_path);
		error_exit("execve error\n");
	}
	close(pipefd[0]);
}

int	main(int argc, char **argv, char **envp)
{
	int		infd;
	int		outfd;
	int		pipefd[2];

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
	wait(0);
	wait(0);
	return (EXIT_SUCCESS);
}
