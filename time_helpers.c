/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 20:27:38 by rbartok           #+#    #+#             */
/*   Updated: 2026/11/23 18:35:19 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "codexion.h"

// 1 second
// │
// ├── 1000 milliseconds
// │
// ├── 1,000,000 microseconds
// │
// └── 1,000,000,000 nanoseconds

long long	get_time(void)
{
	long long			t;
	struct timespec		ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	t = ((long long)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
	return (t);
}

void	compile(t_coder *coder)
{
	long long	simulation_time;

	simulation_time = get_time() - coder->simulation->simulation_start_time;
	pthread_mutex_lock(&coder->simulation->coder_mutex);
	coder->state = COMPILING;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
	safe_print(coder->simulation, simulation_time,
		coder->coder_id, "is compiling\n");
	usleep(coder->simulation->config->time_to_compile * 1000);
	pthread_mutex_lock(&coder->simulation->coder_mutex);
	coder->compilation_count++;
	coder->state = DEBUGGING;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
}

void	debug(t_coder *coder)
{
	long long	simulation_time;

	simulation_time = get_time() - coder->simulation->simulation_start_time;
	safe_print(coder->simulation, simulation_time,
		coder->coder_id, "is debugging\n");
	usleep(coder->simulation->config->time_to_debug * 1000);
	pthread_mutex_lock(&coder->simulation->coder_mutex);
	coder->state = REFACTORING;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
}

void	refactor(t_coder *coder)
{
	long long	simulation_time;

	simulation_time = get_time() - coder->simulation->simulation_start_time;
	safe_print(coder->simulation, simulation_time,
		coder->coder_id, "is refactoring\n");
	usleep(coder->simulation->config->time_to_refactor * 1000);
	pthread_mutex_lock(&coder->simulation->coder_mutex);
	coder->state = CHILLING;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
}
