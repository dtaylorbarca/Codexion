/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 17:07:29 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/30 16:53:22 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int are_dongles_available(t_data *data, int left, int right)
{
    long long now;
    int left_ok;
    int right_ok;

    if (data->dongles[left] != -1 || data->dongles[right] != -1)
        return (0);
    if (left == right)
        return (0);
    now = get_time();
    left_ok = (!data->cooldown[left] || now - data->cooldown[left] >= data->dongle_cooldown);
    right_ok = (!data->cooldown[right] || now - data->cooldown[right] >= data->dongle_cooldown);
    return (left_ok && right_ok);
}

static int acquire_dongles_fifo(t_thread_data *coder, t_data *data,
                                int left, int right)
{
    long long time;

    if (!in_queue(&coder))
        add_to_queue(&coder);
    if (data->queue[0] == coder->id)
    {
        if (are_dongles_available(data, left, right))
        {
            data->dongles[left] = coder->id;
            data->dongles[right] = coder->id;
            pthread_mutex_lock(&coder->mutex_coder);
            coder->last_compile_start = get_time();
            pthread_mutex_unlock(&coder->mutex_coder);
            time = get_time() - data->start_time;
            printf("%lld %d has taken a dongle\n", time, coder->id);
            printf("%lld %d has taken a dongle\n", time, coder->id);
            remove_from_queue(&coder);
            return (1);
        }
    }
    return (0);
}

static int acquire_dongles_edf(t_thread_data *coder, t_data *data,
                               int left, int right)
{
    long long time;

    if (coder->times_compiled >= data->number_of_compiles_required)
        return (0);
    if (!is_in_heap(coder, data))
        heap_push(coder, data);
    if (are_dongles_available(data, left, right))
    {
        if (data->heap_size > 0 && data->heap[0]->id == coder->id)
        {
            heap_pop(data);
            data->dongles[left] = coder->id;
            data->dongles[right] = coder->id;
            pthread_mutex_lock(&coder->mutex_coder);
            coder->last_compile_start = get_time();
            pthread_mutex_unlock(&coder->mutex_coder);
            time = get_time() - data->start_time;
            printf("%lld %d has taken a dongle\n", time, coder->id);
            printf("%lld %d has taken a dongle\n", time, coder->id);
            return (1);
        }
    }
    return (0);
}

int try_acquire_dongles(t_thread_data *coder, t_data *data,
                        int left, int right)
{
    int acquired;
    struct timespec ts;

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
