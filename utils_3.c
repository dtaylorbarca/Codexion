/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 14:04:15 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/10/07 14:04:16 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

int	fifo_blocked(t_data *data, t_thread_data *coder)
{
	int	i;

	i = 0;
	while (data->queue[i] != -1 && data->queue[i] != coder->id)
	{
		if (is_neighbor(data, coder->id, data->queue[i]))
			return (1);
		i++;
	}
	return (0);
}

static int	is_earlier(t_thread_data *a, t_thread_data *b)
{
	long long	da;
	long long	db;

	da = get_deadline(a);
	db = get_deadline(b);
	if (da != db)
		return (da < db);
	if (a->id % 2 != b->id % 2)
		return (a->id % 2);
	return (a->id < b->id);
}

int	edf_blocked(t_data *data, t_thread_data *coder)
{
	int	i;

	i = 0;
	while (i < data->heap_size)
	{
		if (is_neighbor(data, coder->id, data->heap[i]->id)
			&& is_earlier(data->heap[i], coder))
			return (1);
		i++;
	}
	return (0);
}
