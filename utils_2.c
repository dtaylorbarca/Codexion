/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/29 18:49:32 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/08 17:38:11 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	earliest_deadline(t_thread_data **coder)
{
	int			i;
	int			min_id;
	long long	min_time;
	t_data		*data;

	i = 0;
	data = (*coder)->data;
	min_id = -1;
	min_time = LLONG_MAX;
	while (i < data->num_coders)
	{
		if (data->deadlines[i] != -1)
		{
			if (data->deadlines[i] < min_time)
			{
				min_time = data->deadlines[i];
				min_id = i + 1;
			}
		}
		i++;
	}
	return (min_id);
}
