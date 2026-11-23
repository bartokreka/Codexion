/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 20:05:25 by rbartok           #+#    #+#             */
/*   Updated: 2026/11/23 18:34:11 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codexion.h"

void	add_request_time_to_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->simulation->coder_mutex);
	if (coder->state != REQUESTING)
	{
		coder->state = REQUESTING;
		coder->request_time = get_time()
			- coder->simulation->simulation_start_time;
		pthread_mutex_unlock(&coder->simulation->coder_mutex);
		heap_push(&coder->simulation->heap, coder);
		return ;
	}
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
}

int	is_coder_burned_out(t_coder *coders)
{
	int			i;
	int			coders_nr;
	int			coder_burnout;
	long long	now;
	long long	locked_lcs;

	i = 0;
	coders_nr = coders->simulation->config->number_of_coders;
	coder_burnout = coders->simulation->config->time_to_burnout;
	now = get_time() - coders->simulation->simulation_start_time;
	while (i < coders_nr)
	{
		locked_lcs = get_last_compile_start(&coders[i]);
		if (now - locked_lcs >= coder_burnout)
		{
			pthread_mutex_lock(&coders->simulation->simulation_mutex);
			if (!coders->simulation->simulation_finished)
			{
				printf("%lld %d burned out\n", now, coders[i].coder_id);
				coders->simulation->simulation_finished = 1;
			}
			pthread_mutex_unlock(&coders->simulation->simulation_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

int	all_coders_finished(t_coder *coders)
{
	int			i;
	int			coders_nr;
	int			needed;
	int			coder_compilation;

	i = 0;
	coders_nr = coders->simulation->config->number_of_coders;
	needed = coders->simulation->config->number_of_compiles_required;
	while (i < coders_nr)
	{
		coder_compilation = get_compilation_count(&coders[i]);
		if (coder_compilation != needed)
		{
			return (0);
		}
		i++;
	}
	return (1);
}

int	simulation_is_finished(t_simulation *simulation)
{
	int	finished;

	pthread_mutex_lock(&simulation->simulation_mutex);
	finished = simulation->simulation_finished;
	pthread_mutex_unlock(&simulation->simulation_mutex);
	return (finished);
}

void	simulation_finish(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->simulation_mutex);
	simulation->simulation_finished = 1;
	pthread_mutex_unlock(&simulation->simulation_mutex);
}
