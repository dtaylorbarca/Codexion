/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 17:07:29 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/09 12:26:57 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	are_dongles_available(t_data *data, int left, int right)
{
	long long	now;
	int			cooldown_ok;

	if (data->dongles[left] != -1 || data->dongles[right] != -1)
		return (0);
	if (left == right)
		return (0);
	now = get_time();
	cooldown_ok = ((!data->cooldown[left] && !data->cooldown[right])
			|| (now - data->cooldown[left] >= data->dongle_cooldown
				&& now - data->cooldown[right] >= data->dongle_cooldown));
	return (cooldown_ok);
}

static int	acquire_dongles_fifo(t_thread_data *coder, t_data *data,
		int left, int right)
{
	long long	time;

	if (!in_queue(&coder))
		add_to_queue(&coder);
	if (data->queue[0] == coder->id)
	{
		if (are_dongles_available(data, left, right))
		{
			data->dongles[left] = coder->id;
			time = get_time() - data->start_time;
			printf("%lld %d has taken a dongle\n", time, coder->id);
			data->dongles[right] = coder->id;
			time = get_time() - data->start_time;
			printf("%lld %d has taken a dongle\n", time, coder->id);
			remove_from_queue(&coder);
			return (1);
		}
	}
	return (0);
}

static int	are_dongles_available(t_data *data, int left, int right)
{
	long long	now;
	int			cooldown_ok;

	if (data->dongles[left] != -1 || data->dongles[right] != -1)
		return (0);
	if (left == right)
		return (0);
	now = get_time();
	cooldown_ok = ((!data->cooldown[left] && !data->cooldown[right])
			|| (now - data->cooldown[left] >= data->dongle_cooldown
				&& now - data->cooldown[right] >= data->dongle_cooldown));
	return (cooldown_ok);
}

static int	acquire_dongles_edf(t_thread_data *coder, t_data *data,
		int left, int right)
{
	long long	time;

	if (coder->id == earliest_deadline(&coder))
	{
		if (are_dongles_available(data, left, right))
		{
			data->dongles[left] = coder->id;
			time = get_time() - data->start_time;
			printf("%lld %d has taken a dongle\n", time, coder->id);
			data->dongles[right] = coder->id;
			time = get_time() - data->start_time;
			printf("%lld %d has taken a dongle\n", time, coder->id);
			return (1);
		}
	}
	return (0);
}

int	try_acquire_dongles(t_thread_data *coder, t_data *data,
		int left, int right)
{
	int	acquired;

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
	pthread_cond_wait(&data->condition, &data->mutex_data);
	pthread_mutex_unlock(&data->mutex_data);
	return (0);
}
