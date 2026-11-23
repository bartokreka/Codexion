/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 19:02:58 by rbartok           #+#    #+#             */
/*   Updated: 2026/09/24 15:47:08 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "codexion.h"

void	swap_coders(t_coder **a, t_coder **b)
{
	t_coder	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int	has_higher_priority(t_heap *heap, t_coder *a, t_coder *b)
{
	if (!strcmp(heap->scheduler, "fifo"))
		return (a->request_time < b->request_time);
	return (get_last_compile_start(a)
		< get_last_compile_start(b));
}

t_coder	*heap_pop(t_heap *heap)
{
	t_coder	*result;

	pthread_mutex_lock(&heap->mutex);
	if (heap->size == 0)
	{
		pthread_mutex_unlock(&heap->mutex);
		return (NULL);
	}
	result = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		remove_from_heap(heap, 0);
	}
	pthread_mutex_unlock(&heap->mutex);
	return (result);
}

t_coder	*heap_peek(t_heap *heap)
{
	t_coder	*result;

	pthread_mutex_lock(&heap->mutex);
	if (heap->size == 0)
		result = NULL;
	else
		result = heap->items[0];
	pthread_mutex_unlock(&heap->mutex);
	return (result);
}
