/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 20:57:39 by rbartok           #+#    #+#             */
/*   Updated: 2026/09/23 19:24:06 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "codexion.h"

void	*simulation_finish_watcher(void *arg)
{
	t_simulation	*simulation;

	simulation = (t_simulation *)arg;
	while (!simulation_is_finished(simulation))
	{
		if (is_coder_burned_out(simulation->coders)
			|| all_coders_finished(simulation->coders))
			simulation_finish(simulation);
		usleep(500);
	}
	return (NULL);
}

static void	initialize_mutexes(t_simulation *simulation)
{
	pthread_mutex_init(&simulation->dongle_mutex, NULL);
	pthread_mutex_init(&simulation->coder_mutex, NULL);
	pthread_mutex_init(&simulation->simulation_mutex, NULL);
	heap_init(&simulation->heap, simulation->config->number_of_coders,
		simulation->config->scheduler);
}

static void	free_mallocs(t_simulation *simulation,
	t_coder *coders, t_dongle *dongles)
{
	free(dongles);
	free(coders);
	free(simulation);
}

static void	destroy_mutexes_and_cleanup(t_simulation *simulation,
	t_coder *coders, t_dongle *dongles)
{
	pthread_mutex_destroy(&simulation->dongle_mutex);
	pthread_mutex_destroy(&simulation->coder_mutex);
	pthread_mutex_destroy(&simulation->simulation_mutex);
	heap_destroy(&simulation->heap);
	free_mallocs(simulation, coders, dongles);
}

int	main(int argc, char *argv[])
{
	t_config			config;
	t_coder				*coders;
	t_dongle			*dongles;
	t_simulation		*simulation;

	if (!validate_args(argc, argv))
		return (0);
	create_config(argv, &config);
	coders = create_coders(&config);
	if (!coders)
		return (0);
	dongles = create_dongles(&config);
	if (!dongles)
		return (0);
	simulation = create_simulation(coders, dongles, &config);
	if (!simulation)
		return (0);
	initialize_mutexes(simulation);
	create_threads(coders, config.number_of_coders);
	pthread_create(&simulation->thread, NULL,
		simulation_finish_watcher, simulation);
	join_threads(coders);
	pthread_join(simulation->thread, NULL);
	destroy_mutexes_and_cleanup(simulation, coders, dongles);
	return (0);
}
