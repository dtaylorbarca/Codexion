/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 17:07:29 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/10/07 13:55:45 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	are_dongles_available(t_data *data, int left, int right)
{
	long long	now;
	int			left_ok;
	int			right_ok;

	if (data->dongles[left] != -1 || data->dongles[right] != -1)
		return (0);
	if (left == right)
		return (0);
	now = get_time();
	left_ok = (!data->cooldown[left] || now - data->cooldown[left]
			>= data->dongle_cooldown);
	right_ok = (!data->cooldown[right] || now - data->cooldown[right]
			>= data->dongle_cooldown);
	return (left_ok && right_ok);
}

static int	acquire_dongles_fifo(t_thread_data *coder, t_data *data,
		int left, int right)
{
	if (!in_queue(&coder))
		add_to_queue(&coder);
	if (!fifo_blocked(data, coder)
		&& are_dongles_available(data, left, right))
	{
		remove_from_queue(&coder);
		take_dongles(coder, data, left, right);
		pthread_cond_broadcast(&data->condition);
		return (1);
	}
	return (0);
}

static int	acquire_dongles_edf(t_thread_data *coder, t_data *data,
		int left, int right)
{
	heap_push(coder, data);
	if (!edf_blocked(data, coder)
		&& are_dongles_available(data, left, right))
	{
		heap_remove(coder, data);
		take_dongles(coder, data, left, right);
		pthread_cond_broadcast(&data->condition);
		return (1);
	}
	return (0);
}

int	try_acquire_dongles(t_thread_data *coder, t_data *data,
						int left, int right)
{
	int				acquired;
	struct timespec	ts;

	acquired = 0;
	pthread_mutex_lock(&data->mutex_data);
	if (data->simulation_over)
	{
		pthread_mutex_unlock(&data->mutex_data);
		return (-1);
	}
	if (!strcmp("fifo", data->scheduler))
		acquired = acquire_dongles_fifo(coder, data, left, right);
	else if (!strcmp("edf", data->scheduler))
		acquired = acquire_dongles_edf(coder, data, left, right);
	if (acquired)
	{
		pthread_mutex_unlock(&data->mutex_data);
		return (1);
	}
	get_target_time(&ts, 10);
	pthread_cond_timedwait(&data->condition, &data->mutex_data, &ts);
	pthread_mutex_unlock(&data->mutex_data);
	return (0);
}
