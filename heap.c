/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 14:02:38 by dtaylor-          #+#    #+#             */
/*   Updated: 2026/10/07 14:02:39 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

void	heap_push(t_thread_data *coder, t_data *data)
{
	if (is_in_heap(coder, data))
		return ;
	data->heap[data->heap_size] = coder;
	data->heap_size++;
	sift_up(data, data->heap_size - 1);
}

void	heap_remove(t_thread_data *coder, t_data *data)
{
	int	i;

	i = 0;
	while (i < data->heap_size && data->heap[i]->id != coder->id)
		i++;
	if (i == data->heap_size)
		return ;
	data->heap_size--;
	data->heap[i] = data->heap[data->heap_size];
	if (i < data->heap_size)
	{
		sift_up(data, i);
		sift_down(data, i);
	}
}
