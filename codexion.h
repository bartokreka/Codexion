/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 18:01:31 by rbartok           #+#    #+#             */
/*   Updated: 2026/11/23 18:33:15 by rbartok          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stddef.h>
# include <pthread.h>

typedef struct s_coder			t_coder;
typedef struct s_simulation		t_simulation;
typedef struct s_scheduler		t_scheduler;
typedef struct s_heap			t_heap;

typedef struct s_config
{
	int				number_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	char			*scheduler;
}	t_config;

typedef enum e_coder_state
{
	REQUESTING,
	COMPILING,
	DEBUGGING,
	REFACTORING,
	CHILLING
}	t_coder_state;

typedef enum e_dongle_state
{
	AVAILABLE,
	IN_USE,
	COOLDOWN
}	t_dongle_state;

typedef struct s_dongle
{
	int				dongle_id;
	long long		unavailable_until;
	t_dongle_state	state;
}	t_dongle;

typedef struct s_heap
{
	t_coder			**items;
	int				size;
	int				capacity;
	char			*scheduler;
	pthread_mutex_t	mutex;
}	t_heap;

typedef struct s_simulation
{
	t_config		*config;
	t_coder			*coders;
	t_dongle		*dongles;
	long long		simulation_start_time;
	int				simulation_finished;
	pthread_mutex_t	dongle_mutex;
	pthread_mutex_t	simulation_mutex;
	pthread_mutex_t	coder_mutex;
	pthread_t		thread;
	t_heap			heap;
}	t_simulation;

typedef struct s_coder
{
	int				coder_id;
	t_coder_state	state;
	long long		last_compile_start;
	long long		request_time;
	int				compilation_count;
	pthread_t		thread;
	t_simulation	*simulation;
}	t_coder;

int				is_non_negative_number(char *str);
int				validate_args(int argc, char *argv[]);
void			create_config(char *argv[], t_config *config);
void			print_config(t_config *config);
long long		get_time(void);
void			debug(t_coder *coder);
void			compile(t_coder *coder);
void			refactor(t_coder *coder);
t_coder			*create_coders(t_config *config);
void			create_threads(t_coder *coders, int number_of_coders);
void			join_threads(t_coder *coders);
t_dongle		*create_dongles(t_config *config);
t_simulation	*create_simulation(t_coder *coders,
					t_dongle *dongles, t_config *config);
int				acquire_dongles(t_coder *coder);
void			release_dongles(t_coder *coder);
t_coder			*get_next_coder(t_simulation *simulation);
void			add_request_time_to_coder(t_coder *coder);
int				is_coder_burned_out(t_coder *coders);
int				simulation_is_finished(t_simulation *simulation);
void			simulation_finish(t_simulation *simulation);
void			*simulation_finish_watcher(void *arg);
int				all_coders_finished(t_coder *coders);
int				get_compilation_count(t_coder *coder);
long long		get_last_compile_start(t_coder *coder);
t_coder_state	get_coder_status(t_coder *coder);
void			heap_init(t_heap *heap, int capacity, char *scheduler);
void			heap_destroy(t_heap *heap);
int				has_higher_priority(t_heap *heap, t_coder *a, t_coder *b);
void			swap_coders(t_coder **a, t_coder **b);
void			add_to_heap(t_heap *heap, int index);
int				heap_push(t_heap *heap, t_coder *coder);
void			remove_from_heap(t_heap *heap, int index);
t_coder			*heap_pop(t_heap *heap);
t_coder			*heap_peek(t_heap *heap);
void			safe_print(t_simulation *sim, long long when, int id, char *m);

#endif /* CODEXION_H */