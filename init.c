/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 17:08:29 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/28 16:58:18 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_args(char **argv)
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if ((i == 6 || i == 7) && argv[i][0] == '0' && strlen(argv[i]) == 1)
		{
			i++;
			continue ;
		}
		if (!num_check(argv[i]))
			return (0);
		i++;
	}
	if (strcmp("fifo", argv[8]) && strcmp("edf", argv[8]))
		return (0);
	return (1);
}

static int	alloc_data_buffers(t_data **data)
{
	(*data)->dongles = malloc(((*data)->num_coders + 1) * sizeof(int));
	if (!(*data)->dongles)
		return (0);
	memset((*data)->dongles, -1, (*data)->num_coders * sizeof(int));
	(*data)->dongles[(*data)->num_coders] = 0;
	(*data)->queue = malloc((*data)->num_coders * sizeof(int) + 1);
	if (!(*data)->queue)
		return (0);
	memset((*data)->queue, -1, (*data)->num_coders * sizeof(int));
	(*data)->cooldown = malloc((*data)->num_coders * sizeof(long long) + 1);
	if (!(*data)->cooldown)
		return (0);
	memset((*data)->cooldown, 0, (*data)->num_coders * sizeof(long long));
	(*data)->heap = malloc(sizeof(t_thread_data *) * (*data)->num_coders);
	if (!(*data)->heap)
		return (0);
	memset((*data)->heap, 0, (*data)->num_coders * sizeof(t_thread_data *));
	return (1);
}

int	data_setup(t_data **data, char **argv)
{
	if (!check_args(argv))
		return (0);
	(*data)->num_coders = num_check(argv[1]);
	(*data)->time_to_burnout = num_check(argv[2]);
	(*data)->time_to_compile = num_check(argv[3]);
	(*data)->time_to_debug = num_check(argv[4]);
	(*data)->time_to_refactor = num_check(argv[5]);
	if (argv[6][0] == '0' && strlen(argv[6]) == 1)
		(*data)->number_of_compiles_required = 0;
	else
		(*data)->number_of_compiles_required = num_check(argv[6]);
	if (argv[7][0] == '0' && strlen(argv[7]) == 1)
		(*data)->dongle_cooldown = 0;
	else
		(*data)->dongle_cooldown = num_check(argv[7]);
	(*data)->scheduler = argv[8];
	(*data)->simulation_over = 0;
	(*data)->threads_done = 0;
	return (alloc_data_buffers(data));
}

void	free_data(t_data **data)
{
	free((*data)->dongles);
	free((*data)->queue);
	free((*data)->cooldown);
	free((*data)->heap);
}
