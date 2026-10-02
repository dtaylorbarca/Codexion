/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 16:38:31 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/09/23 16:41:05 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_nodes(t_thread_data **a, t_thread_data **b)
{
	t_thread_data	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_push(t_thread_data *coder, t_data *data)
{
	int	i;
	int	parent;

	if (is_in_heap(coder, data))
		return ;
	i = data->heap_size;
	data->heap[i] = coder;
	data->heap_size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (get_deadline(data->heap[i]) < get_deadline(data->heap[parent]))
		{
			swap_nodes(&data->heap[i], &data->heap[parent]);
			i = parent;
		}
		else
			break ;
	}
}

static int	get_smallest_child(t_data *data, int i)
{
	int	left;
	int	right;
	int	smallest;

	smallest = i;
	left = 2 * i + 1;
	right = 2 * i + 2;
	if (left < data->heap_size
		&& get_deadline(data->heap[left]) < get_deadline(data->heap[smallest]))
		smallest = left;
	if (right < data->heap_size
		&& get_deadline(data->heap[right]) < get_deadline(data->heap[smallest]))
		smallest = right;
	return (smallest);
}

void	heap_pop(t_data *data)
{
	int	i;
	int	smallest;

	if (data->heap_size <= 0)
		return ;
	data->heap[0] = data->heap[data->heap_size - 1];
	data->heap_size--;
	i = 0;
	while (1)
	{
		smallest = get_smallest_child(data, i);
		if (smallest == i)
			break ;
		swap_nodes(&data->heap[i], &data->heap[smallest]);
		i = smallest;
	}
}

int	is_in_heap(t_thread_data *coder, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->heap_size)
	{
		if (data->heap[i]->id == coder->id)
			return (1);
		i++;
	}
	return (0);
}
