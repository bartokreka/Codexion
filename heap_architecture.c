/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_architecture.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 19:02:26 by rbartok           #+#    #+#             */
/*   Updated: 2026/09/23 19:31:01 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "codexion.h"

void	heap_init(t_heap *heap, int capacity, char *scheduler)
{
	heap->items = malloc(sizeof(t_coder *) * capacity);
	if (!heap->items)
		return ;
	heap->size = 0;
	heap->capacity = capacity;
	heap->scheduler = scheduler;
	pthread_mutex_init(&heap->mutex, NULL);
}

void	heap_destroy(t_heap *heap)
{
	if (heap->items)
		free(heap->items);
	pthread_mutex_destroy(&heap->mutex);
}

void	add_to_heap(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!has_higher_priority(heap, heap->items[index],
				heap->items[parent]))
			break ;
		swap_coders(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
}

int	heap_push(t_heap *heap, t_coder *coder)
{
	pthread_mutex_lock(&heap->mutex);
	if (heap->size >= heap->capacity)
	{
		pthread_mutex_unlock(&heap->mutex);
		return (0);
	}
	heap->items[heap->size] = coder;
	heap->size++;
	add_to_heap(heap, heap->size - 1);
	pthread_mutex_unlock(&heap->mutex);
	return (1);
}

void	remove_from_heap(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = index * 2 + 1;
		right = index * 2 + 2;
		best = index;
		if (left < heap->size
			&& has_higher_priority(heap, heap->items[left],
				heap->items[best]))
			best = left;
		if (right < heap->size
			&& has_higher_priority(heap, heap->items[right],
				heap->items[best]))
			best = right;
		if (best == index)
			break ;
		swap_coders(&heap->items[index], &heap->items[best]);
		index = best;
	}
}
