/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 20:05:25 by rbartok           #+#    #+#             */
/*   Updated: 2026/09/23 17:13:39 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "codexion.h"

int	get_compilation_count(t_coder *coder)
{
	int	compilation_count;

	pthread_mutex_lock(&coder->simulation->coder_mutex);
	compilation_count = coder->compilation_count;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
	return (compilation_count);
}

long long	get_last_compile_start(t_coder *coder)
{
	long long	last_compile_start;

	pthread_mutex_lock(&coder->simulation->coder_mutex);
	last_compile_start = coder->last_compile_start;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
	return (last_compile_start);
}

t_coder_state	get_coder_status(t_coder *coder)
{
	t_coder_state	coder_state;

	pthread_mutex_lock(&coder->simulation->coder_mutex);
	coder_state = coder->state;
	pthread_mutex_unlock(&coder->simulation->coder_mutex);
	return (coder_state);
}
