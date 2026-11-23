/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/16 21:03:54 by username          #+#    #+#             */
/*   Updated: 2026/09/23 16:48:29 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "codexion.h"

static int	wait_for_dongles(t_coder *coder)
{
	while (!acquire_dongles(coder))
	{
		if (simulation_is_finished(coder->simulation))
			return (0);
		usleep(500);
	}
	if (simulation_is_finished(coder->simulation))
	{
		release_dongles(coder);
		return (0);
	}
	return (1);
}

static void	*worker(void *arg)
{
	t_coder			*coder;
	t_simulation	*simulation;

	coder = (t_coder *)arg;
	simulation = coder->simulation;
	while (!simulation_is_finished(simulation)
		&& get_compilation_count(coder)
		< simulation->config->number_of_compiles_required)
	{
		if (!wait_for_dongles(coder))
			return (NULL);
		compile(coder);
		release_dongles(coder);
		if (simulation_is_finished(coder->simulation))
			return (NULL);
		debug(coder);
		if (simulation_is_finished(coder->simulation))
			return (NULL);
		refactor(coder);
	}
	return (NULL);
}

void	create_threads(t_coder *coders, int number_of_coders)
{
	int	i;

	i = 0;
	while (i < number_of_coders)
	{
		pthread_create(&coders[i].thread, NULL, worker, &coders[i]);
		i++;
	}
}

t_coder	*create_coders(t_config *config)
{
	int		i;
	t_coder	*coders;

	coders = malloc(sizeof(t_coder) * config->number_of_coders);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < config->number_of_coders)
	{
		coders[i].coder_id = i + 1;
		coders[i].state = CHILLING;
		coders[i].last_compile_start = 0;
		coders[i].request_time = 0;
		coders[i].compilation_count = 0;
		coders[i].simulation = NULL;
		i++;
	}
	return (coders);
}

void	join_threads(t_coder *coders)
{
	int	i;

	i = 0;
	while (i < coders->simulation->config->number_of_coders)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
}
