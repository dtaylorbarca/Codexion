/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 17:14:16 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/30 17:00:21 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

void get_target_time(struct timespec *ts, long long ms_to_wait)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    ts->tv_sec = tv.tv_sec + (ms_to_wait / 1000);
    ts->tv_nsec = (tv.tv_usec * 1000) + ((ms_to_wait % 1000) * 1000000);
    if (ts->tv_nsec >= 1000000000)
    {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000;
    }
}

void precise_sleep(long long duration, t_data *data)
{
    long long start;

    start = get_time();
    while (!data->simulation_over)
    {
        if (get_time() - start >= duration)
            break;
        usleep(500);
    }
}

void check_and_mark_completion(t_thread_data *coder, t_data *data)
{
    if (coder->times_compiled >= data->number_of_compiles_required)
    {
        if (coder->times_compiled == data->number_of_compiles_required)
            data->threads_done++;
        if (data->threads_done >= data->num_coders)
        {
            data->simulation_over = 1;
            pthread_cond_broadcast(&data->condition);
        }
    }
}

long long get_deadline(t_thread_data *coder)
{
    long long last_start;

    pthread_mutex_lock(&coder->mutex_coder);
    last_start = coder->last_compile_start;
    pthread_mutex_unlock(&coder->mutex_coder);
    return (last_start + coder->data->time_to_burnout);
}
