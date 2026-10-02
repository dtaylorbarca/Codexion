/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 18:21:21 by username          #+#    #+#             */
/*   Updated: 2026/09/30 16:49:34 by dtaylor-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H

# define CODEXION_H

# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>

/* Strucutres */

typedef struct s_thread_data	t_thread_data;

typedef struct s_data
{
	int				num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				number_of_compiles_required;
	long long		dongle_cooldown;
	char			*scheduler;
	int				simulation_over;
	long long		start_time;
	int				*dongles;
	int				*queue;
	int				threads_done;
	long long		*cooldown;
	int				heap_size;
	t_thread_data	**heap;
	pthread_mutex_t	mutex_data;
	pthread_cond_t	condition;
	pthread_t		monitor_thread;
}	t_data;

struct s_thread_data
{
	int				id;
	long long		last_compile_start;
	int				times_compiled;
	pthread_t		thread_id;
	pthread_mutex_t	mutex_coder;
	t_data			*data;
};

/* Utility */

long long	get_time(void);
int			num_check(const char *num);
void		add_to_queue(t_thread_data **coder);
void		remove_from_queue(t_thread_data **coder);
int			in_queue(t_thread_data **coder);
void		get_target_time(struct timespec *ts, long long ms_to_wait);
void		precise_sleep(long long duration, t_data *data);
void		check_and_mark_completion(t_thread_data *coder, t_data *data);
long long	get_deadline(t_thread_data *coder);

/* Heap Utility */

void		heap_push(t_thread_data *coder, t_data *data);
void		heap_pop(t_data *data);
int			is_in_heap(t_thread_data *coder, t_data *data);

/* Scheduer & Acquiring */

int			try_acquire_dongles(t_thread_data *coder, t_data *data,
				int left, int right);

/* Routine Logic */

void		*routine(void *arg);

/* Monitor Logic */

void		*monitor(void *arg);

/* Data Setup & Memory */

int			data_setup(t_data **data, char **argv);
void		free_data(t_data **data);

#endif
