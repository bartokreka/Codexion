/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 18:29:20 by rbartok           #+#    #+#             */
/*   Updated: 2026/09/24 14:46:24 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codexion.h"

static void	assign_simulation(t_coder *coders,
		t_simulation *simulation, int number_of_coders)
{
	int	i;

	i = 0;
	while (i < number_of_coders)
	{
		coders[i].simulation = simulation;
		i++;
	}
}

t_simulation	*create_simulation(t_coder *coders,
	t_dongle *dongles, t_config *config)
{
	t_simulation	*simulation;

	simulation = malloc(sizeof(t_simulation));
	if (!simulation)
		return (NULL);
	simulation->coders = coders;
	simulation->dongles = dongles;
	simulation->config = config;
	simulation->simulation_start_time = get_time();
	simulation->simulation_finished = 0;
	assign_simulation(coders, simulation, config->number_of_coders);
	return (simulation);
}

t_coder	*get_next_coder(t_simulation *simulation)
{
	return (heap_peek(&simulation->heap));
}

// t_coder	*get_next_coder(t_simulation *simulation)
// {
// 	if (strcmp(simulation->config->scheduler, "fifo") == 0)
// 		return (schedule_fifo(simulation));
// 	else
// 		return (schedule_edf(simulation));
// }

// t_coder	*schedule_fifo(t_simulation *simulation)
// {
// 	t_coder	*chosen_one;
// 	int		i;
// 	int		coder_nr;

// 	i = 0;
// 	chosen_one = NULL;
// 	coder_nr = simulation->config->number_of_coders;
// 	while (i < coder_nr)
// 	{
// 		if (get_coder_status(&simulation->coders[i]) == REQUESTING)
// 		{
// 			if (chosen_one == NULL)
// 				chosen_one = &simulation->coders[i];
// 			else if (simulation->coders[i].request_time
// 				< chosen_one->request_time)
// 				chosen_one = &simulation->coders[i];
// 		}
// 		i++;
// 	}
// 	return (chosen_one);
// }

// t_coder	*schedule_edf(t_simulation *simulation)
// {
// 	t_coder		*chosen_one;
// 	int			i;

// 	i = 0;
// 	chosen_one = NULL;
// 	while (i < simulation->config->number_of_coders)
// 	{
// 		if (simulation->coders[i].state == REQUESTING)
// 		{
// 			if (chosen_one == NULL)
// 				chosen_one = &simulation->coders[i];
// 			else if (get_last_compile_start(&simulation->coders[i])
// 				< get_last_compile_start(chosen_one))
// 				chosen_one = &simulation->coders[i];
// 		}
// 		i++;
// 	}
// 	return (chosen_one);
// }
