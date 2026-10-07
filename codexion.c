/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 12:23:13 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/10/07 13:53:51 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_coders(t_thread_data *coders, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		coders[i].id = i + 1;
		coders[i].data = data;
		coders[i].last_compile_start = get_time();
		coders[i].times_compiled = 0;
		pthread_mutex_init(&coders[i].mutex_coder, NULL);
		i++;
	}
}

static int	start_threads(t_thread_data *coders, t_data *data)
{
	int	i;

	if (pthread_create(&data->monitor_thread, NULL, &monitor,
			(void *)coders) != 0)
	{
		printf("Failed to create thread");
		return (0);
	}
	i = 0;
	while (i < data->num_coders)
	{
		if (pthread_create(&coders[i].thread_id, NULL,
				&routine, &coders[i]) != 0)
		{
			printf("Failed to create coder thread");
			return (0);
		}
		i++;
	}
	return (1);
}

static void	cleanup_all(t_thread_data *coders, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	pthread_join(data->monitor_thread, NULL);
	i = 0;
	while (i < data->num_coders)
	{
		pthread_mutex_destroy(&coders[i].mutex_coder);
		i++;
	}
	pthread_cond_destroy(&data->condition);
	pthread_mutex_destroy(&data->mutex_data);
	free_data(&data);
	free(data);
	free(coders);
}

int	main(int argc, char **argv)
{
	t_thread_data	*coders;
	t_data			*data;

	if (argc != 9)
		return (printf("8 arguments must be provided"), 1);
	data = (t_data *)malloc(sizeof(t_data));
	if (!data)
		return (1);
	memset(data, 0, sizeof(t_data));
	if (!data_setup(&data, argv))
		return (printf("Syntax error"), 1);
	init_queue(data);
	coders = (t_thread_data *)malloc(data->num_coders * sizeof(t_thread_data));
	if (!coders)
		return (1);
	pthread_mutex_init(&data->mutex_data, NULL);
	pthread_cond_init(&data->condition, NULL);
	data->start_time = get_time();
	init_coders(coders, data);
	if (!start_threads(coders, data))
		return (1);
	cleanup_all(coders, data);
	return (0);
}
