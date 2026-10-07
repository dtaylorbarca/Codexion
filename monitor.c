/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 17:08:00 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/10/07 13:46:41 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_threads_done(t_thread_data *coders, t_data *data)
{
	int	j;
	int	done;

	j = 0;
	done = 0;
	pthread_mutex_lock(&data->mutex_data);
	while (j < data->num_coders)
	{
		if (coders[j].times_compiled >= data->number_of_compiles_required)
			done++;
		j++;
	}
	if (done == data->num_coders)
	{
		data->simulation_over = 1;
		pthread_cond_broadcast(&data->condition);
	}
	pthread_mutex_unlock(&data->mutex_data);
	return (done == data->num_coders);
}

static int	burnout_found(t_thread_data *coder, t_data *data)
{
	if (data->number_of_compiles_required > 0
		&& coder->times_compiled >= data->number_of_compiles_required)
		return (0);
	return (get_time() - coder->last_compile_start >= data->time_to_burnout);
}

static int	check_one(t_thread_data *coder, t_data *data)
{
	int	res;

	res = 0;
	pthread_mutex_lock(&data->mutex_data);
	pthread_mutex_lock(&coder->mutex_coder);
	if (data->simulation_over)
		res = 1;
	else if (burnout_found(coder, data))
	{
		data->simulation_over = 1;
		pthread_cond_broadcast(&data->condition);
		printf("%lld %d burned out\n",
			get_time() - data->start_time, coder->id);
		res = 1;
	}
	pthread_mutex_unlock(&coder->mutex_coder);
	pthread_mutex_unlock(&data->mutex_data);
	return (res);
}

static int	check_burnout(t_thread_data *coders, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		if (check_one(&coders[i], data))
			return (1);
		i++;
	}
	return (0);
}

void	*monitor(void *arg)
{
	t_data			*data;
	t_thread_data	*coders;

	coders = (t_thread_data *)arg;
	data = coders[0].data;
	while (1)
	{
		pthread_mutex_lock(&data->mutex_data);
		if (data->simulation_over)
			return (pthread_mutex_unlock(&data->mutex_data), NULL);
		pthread_mutex_unlock(&data->mutex_data);
		if (check_threads_done(coders, data))
			return (NULL);
		if (check_burnout(coders, data))
			return (NULL);
		usleep(2000);
	}
	return (NULL);
}
