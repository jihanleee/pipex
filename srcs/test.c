#include "pipex.h"

int	main(int argc, char **argv, char **envp)
{
	int		fd;
	int		du;
	int		stdout_save;
	pid_t	cpid;
	int		bytes_read;
	int		pipedes[2];
	char	buf[1];

	fd = open("test", O_RDWR);
	stdout_save = dup(1);
	pipe(pipedes);
	close(1);
	ft_printf("fd%d du%d\n", fd, du);
	dup2(pipedes[1], 1);
	write(1, "voilala", 7);
	close(pipedes[1]);
	while (1)
	{
		bytes_read = read(pipedes[0], buf, 1);
		if (bytes_read == 0)
			break ;
		/* write(stdout_save, buf, 1); */
	}
	close(pipedes[0]);
	_exit(EXIT_SUCCESS);
	return (0);
}