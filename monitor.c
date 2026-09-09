/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 17:08:00 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/08 17:08:01 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_threads_done(t_thread_data *coders, t_data *data)
{
	int	j;

	j = 0;
	data->threads_done = 0;
	pthread_mutex_lock(&data->mutex_data);
	while (j < data->num_coders)
	{
		if (coders[j].times_compiled > data->number_of_compiles_required)
			data->threads_done++;
		j++;
	}
	if (data->threads_done == data->num_coders)
	{
		data->simulation_over = 1;
		pthread_cond_broadcast(&data->condition);
		printf("Ended due to all coders compiling\n");
		pthread_mutex_unlock(&data->mutex_data);
		return (1);
	}
	pthread_mutex_unlock(&data->mutex_data);
	return (0);
}

static int	check_burnout(t_thread_data *coders, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		pthread_mutex_lock(&coders[i].mutex_coder);
		if (get_time() - coders[i].last_compile_start >= data->time_to_burnout)
		{
			pthread_mutex_lock(&data->mutex_data);
			data->simulation_over = 1;
			pthread_cond_broadcast(&data->condition);
			pthread_mutex_unlock(&data->mutex_data);
			printf("%lld %d burned out\n",
				get_time() - data->start_time, coders[i].id);
			pthread_mutex_unlock(&coders[i].mutex_coder);
			return (1);
		}
		pthread_mutex_unlock(&coders[i].mutex_coder);
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
		if (check_threads_done(coders, data))
			return (NULL);
		if (check_burnout(coders, data))
			return (NULL);
		usleep(2000);
	}
	return (NULL);
}
