/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_actions.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 21:38:32 by rbartok           #+#    #+#             */
/*   Updated: 2026/11/23 18:31:28 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "codexion.h"

t_dongle	*create_dongles(t_config *config)
{
	t_dongle	*dongles;
	int			i;

	dongles = malloc(sizeof(t_dongle) * config->number_of_coders);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		dongles[i].state = AVAILABLE;
		dongles[i].dongle_id = i + 1;
		dongles[i].unavailable_until = 0;
		i++;
	}
	return (dongles);
}

static void	is_cooldown(t_dongle *d1, t_dongle *d2)
{
	long long	current_time;

	current_time = get_time();
	if (d1->state == COOLDOWN && current_time >= d1->unavailable_until)
		d1->state = AVAILABLE;
	if (d2->state == COOLDOWN && current_time >= d2->unavailable_until)
		d2->state = AVAILABLE;
}

static int	use_dongles(t_coder *coder)
{
	t_dongle	*d1;
	t_dongle	*d2;
	long long	when;

	add_request_time_to_coder(coder);
	if (get_next_coder(coder->simulation) != coder)
		return (0);
	d1 = &coder->simulation->dongles[coder->coder_id - 1];
	if (coder->coder_id == coder->simulation->config->number_of_coders)
		d2 = &coder->simulation->dongles[0];
	else
		d2 = &coder->simulation->dongles[coder->coder_id];
	is_cooldown(d1, d2);
	if (d1->state != AVAILABLE || d2->state != AVAILABLE)
		return (0);
	heap_pop(&coder->simulation->heap);
	when = get_time() - coder->simulation->simulation_start_time;
	d1->state = IN_USE;
	safe_print(coder->simulation, when, coder->coder_id, "has taken a dongle\n");
	d2->state = IN_USE;
	safe_print(coder->simulation, when, coder->coder_id, "has taken a dongle\n");
	pthread_mutex_lock(&coder->simulation->coder_mutex);
	coder->last_compile_start = when;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
	return (1);
}

int	acquire_dongles(t_coder *coder)
{
	int			is_success;

	if (coder->simulation->config->number_of_coders == 1)
		return (0);
	pthread_mutex_lock(&coder->simulation->dongle_mutex);
	is_success = use_dongles(coder);
	pthread_mutex_unlock(&coder->simulation->dongle_mutex);
	return (is_success);
}

void	release_dongles(t_coder *coder)
{
	t_dongle	*d1;
	t_dongle	*d2;
	int			dongle_nr;
	long long	needed_time;

	d1 = &coder->simulation->dongles[coder->coder_id - 1];
	dongle_nr = coder->simulation->config->number_of_coders;
	if (coder->coder_id == dongle_nr)
		d2 = &coder->simulation->dongles[0];
	else
		d2 = &coder->simulation->dongles[coder->coder_id];
	pthread_mutex_lock(&coder->simulation->dongle_mutex);
	needed_time = get_time() + coder->simulation->config->dongle_cooldown;
	d1->unavailable_until = needed_time;
	d1->state = COOLDOWN;
	d2->unavailable_until = needed_time;
	d2->state = COOLDOWN;
	pthread_mutex_unlock(&coder->simulation->dongle_mutex);
}
