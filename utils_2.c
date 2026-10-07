/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 17:14:16 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/10/07 14:04:28 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

void	get_target_time(struct timespec *ts, long long ms_to_wait)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec + (ms_to_wait / 1000);
	ts->tv_nsec = (tv.tv_usec * 1000) + ((ms_to_wait % 1000) * 1000000);
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec += 1;
		ts->tv_nsec -= 1000000000;
	}
}

void	precise_sleep(long long duration, t_data *data)
{
	long long	start;

	start = get_time();
	while (!data->simulation_over)
	{
		if (get_time() - start >= duration)
			break ;
		usleep(500);
	}
}

long long	get_deadline(t_thread_data *coder)
{
	long long	last_start;

	pthread_mutex_lock(&coder->mutex_coder);
	last_start = coder->last_compile_start;
	pthread_mutex_unlock(&coder->mutex_coder);
	return (last_start + coder->data->time_to_burnout);
}

void	take_dongles(t_thread_data *coder, t_data *data, int left, int right)
{
	long long	now;

	data->dongles[left] = coder->id;
	data->dongles[right] = coder->id;
	now = get_time();
	pthread_mutex_lock(&coder->mutex_coder);
	coder->last_compile_start = now;
	pthread_mutex_unlock(&coder->mutex_coder);
	printf("%lld %d has taken a dongle\n", now - data->start_time, coder->id);
	printf("%lld %d has taken a dongle\n", now - data->start_time, coder->id);
}

static int	is_neighbor(t_data *data, int a, int b)
{
	if (a == b)
		return (0);
	return (b == a % data->num_coders + 1
		|| a == b % data->num_coders + 1);
}
