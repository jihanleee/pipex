#include "pipex.h"


int	main(int argc, char **argv, char **envp)
{
	int		infd;
	int		pipefd[2];
	char	buf;
	pid_t	cpid;
/* 	if (argc < 5)
		return ((perror("Wrong arguments\n"), 0)); */
	if (pipe(pipefd) == -1)
		perror("pipe\n");
	infd = open(argv[1], O_RDONLY);
	if (infd == -1)
	{
		perror("weird ass file\n");
		exit(0);
	}
	cpid = fork();
	if (cpid == 0)
	{
		ft_printf("hello im the child");
		close(pipefd[0]);
		dup2(infd, 0);
		while (read(0, &buf, 1))
			write (pipefd[1], &buf, 1);
		exit(0);
	}
	cpid = fork();
	if (cpid == 0)
	{
		while (read(pipefd[0], &buf, 1))
			write (1, &buf, 1);
		wait(0);
		exit(0);
	}
	return (0);
}
