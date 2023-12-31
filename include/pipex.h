/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihalee <jihalee@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 18:36:18 by jihalee           #+#    #+#             */
/*   Updated: 2023/08/08 17:18:07 by jihalee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>
# include "libft.h"
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <errno.h>

/*utils.c*/
void	free_arrays(char **str);
void	error_exit(char *str);

#endif