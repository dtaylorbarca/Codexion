/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 17:08:02 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/30 17:15:41 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	do_coder_actions(t_thread_data *coder, t_data *data)
{
	pthread_mutex_lock(&data->mutex_data);
	if (data->simulation_over)
	{
		pthread_mutex_unlock(&data->mutex_data);
		return (1);
	}
	printf("%lld %d is compiling\n", get_time() - data->start_time, coder->id);
	pthread_mutex_unlock(&data->mutex_data);
    precise_sleep(data->time_to_compile, data);
	pthread_mutex_lock(&coder->mutex_coder);
	coder->times_compiled++;
	pthread_mutex_unlock(&coder->mutex_coder);
	return (0);
}

static int	release_and_debug(t_thread_data *coder, t_data *data,
		int left, int right)
{
	pthread_mutex_lock(&data->mutex_data);
	data->dongles[left] = -1;
	data->dongles[right] = -1;
	data->cooldown[left] = get_time();
	data->cooldown[right] = get_time();
	check_and_mark_completion(coder, data);
	pthread_cond_broadcast(&data->condition);
	if (data->simulation_over)
	{
		pthread_mutex_unlock(&data->mutex_data);
		return (1);
	}
	printf("%lld %d is debugging\n", get_time() - data->start_time, coder->id);
	pthread_mutex_unlock(&data->mutex_data);
	precise_sleep(data->time_to_debug, data);
	return (0);
}

static int	do_rest_of_routine(t_thread_data *coder, t_data *data,
		int left, int right)
{
	if (release_and_debug(coder, data, left, right))
		return (1);
	pthread_mutex_lock(&data->mutex_data);
	if (data->simulation_over)
	{
		pthread_mutex_unlock(&data->mutex_data);
		return (1);
	}
	printf("%lld %d is refactoring\n",
		get_time() - data->start_time, coder->id);
	pthread_mutex_unlock(&data->mutex_data);
	precise_sleep(data->time_to_refactor, data);
	return (0);
}

static int	run_routine_step(t_thread_data *coder, t_data *data,
		int left, int right)
{
	int	res;

	res = 0;
	while (!res)
		res = try_acquire_dongles(coder, data, left, right);
	if (res == -1)
		return (1);
	if (do_coder_actions(coder, data))
		return (1);
	if (do_rest_of_routine(coder, data, left, right))
		return (1);
	return (0);
}

void	*routine(void *arg)
{
	t_thread_data	*coder;
	t_data			*data;
	int				left;
	int				right;

	coder = (t_thread_data *)arg;
	data = coder->data;
	left = (coder->id - 1 + data->num_coders) % data->num_coders;
	right = coder->id % data->num_coders;
	pthread_mutex_lock(&coder->mutex_coder);
	coder->last_compile_start = get_time();
	pthread_mutex_unlock(&coder->mutex_coder);
	while (1)
	{
		if (run_routine_step(coder, data, left, right))
			break ;
	}
	return (NULL);
}
