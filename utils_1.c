/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 18:21:24 by username          #+#    #+#             */
/*   Updated: 2026/10/07 12:44:17 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}

int	num_check(const char *num)
{
	int	i;
	int	number;

	i = 0;
	while (num[i])
	{
		if ('9' < num[i] || num[i] < '0')
			return (0);
		i++;
	}
	number = atoi(num);
	if (number < 1)
		return (0);
	return (number);
}

void	add_to_queue(t_thread_data **coder)
{
	int	i;

	i = 0;
	while ((*coder)->data->queue[i] != -1 && i < (*coder)->data->num_coders)
		i++;
	if (i < (*coder)->data->num_coders)
		(*coder)->data->queue[i] = (*coder)->id;
}

int	in_queue(t_thread_data **coder)
{
	int		i;
	int		id;
	int		*queue;
	t_data	*data;

	i = 0;
	id = (*coder)->id;
	data = (*coder)->data;
	queue = data->queue;
	while (i < data->num_coders)
	{
		if (queue[i] == id)
			return (1);
		i++;
	}
	return (0);
}

void	remove_from_queue(t_thread_data **coder)
{
	int		i;
	int		*queue;
	t_data	*data;

	i = 0;
	data = (*coder)->data;
	queue = data->queue;
	while (i < data->num_coders && queue[i] != (*coder)->id)
		i++;
	while (i + 1 < data->num_coders)
	{
		queue[i] = queue[i + 1];
		i++;
	}
	if (i < data->num_coders)
		queue[i] = -1;
}
