/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg_validation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 18:12:38 by rbartok           #+#    #+#             */
/*   Updated: 2026/09/23 11:29:52 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "codexion.h"

int	is_non_negative_number(char *str)
{
	int	i;

	i = 0;
	if (str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	validate_number_of_args(char *argv[])
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if (!is_non_negative_number(argv[i]))
		{
			printf("All numbers must be non-negative integers!");
			return (0);
		}
		i++;
	}
	return (1);
}

int	validate_args(int argc, char *argv[])
{
	if (argc != 9)
	{
		printf("Not the exact number of arguments");
		return (0);
	}
	if (strcmp(argv[1], "0") == 0)
	{
		printf("There must be at least 1 coder!");
		return (0);
	}
	if (!validate_number_of_args(argv))
		return (0);
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		printf("The arbitration policy must be fifo or edf");
		return (0);
	}
	return (1);
}

void	create_config(char *argv[], t_config *config)
{
	config->number_of_coders = atoi(argv[1]);
	config->time_to_burnout = atoi(argv[2]);
	config->time_to_compile = atoi(argv[3]);
	config->time_to_debug = atoi(argv[4]);
	config->time_to_refactor = atoi(argv[5]);
	config->number_of_compiles_required = atoi(argv[6]);
	config->dongle_cooldown = atoi(argv[7]);
	config->scheduler = argv[8];
}

void	print_config(t_config *config)
{
	printf("Simulation configurations:\n");
	printf("===========================\n");
	printf("Number of coders: %d\n", config->number_of_coders);
	printf("Time to burnout: %d\n", config->time_to_burnout);
	printf("Time to compile: %d\n", config->time_to_compile);
	printf("Time to debug: %d\n", config->time_to_debug);
	printf("Time to refactor: %d\n", config->time_to_refactor);
	printf("Number of compiles required: %d\n",
		config->number_of_compiles_required);
	printf("Dongle cooldown: %d\n", config->dongle_cooldown);
	printf("Scheduler: %s\n", config->scheduler);
	printf("===========================\n\n");
}
