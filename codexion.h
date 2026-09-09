/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtaylor- <dtaylor-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 18:21:21 by username          #+#    #+#             */
/*   Updated: 2026/09/09 12:23:03 by dtaylor-         ###   ########.fr       */
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
	long long		*deadlines;
	int				threads_done;
	long long		*cooldown;
	int				*have_compiled;
	pthread_mutex_t	mutex_data;
	pthread_cond_t	condition;
	pthread_t		thread_id;
}	t_data;

typedef struct s_thread_data
{
	int				id;
	long long		last_compile_start;
	int				times_compiled;
	pthread_t		thread_id;
	pthread_mutex_t	mutex_coder;
	t_data			*data;
}	t_thread_data;

/* Utility */

long long	get_time(void);
int			num_check(const char *num);
void		add_to_queue(t_thread_data **coder);
void		remove_from_queue(t_thread_data **coder);
int			in_queue(t_thread_data **coder);
int			earliest_deadline(t_thread_data **coder);

/* Scheduer & Acquiring */

int			try_acquire_dongles(t_thread_data *coder, t_data *data,
				int left, int right);

/* Routine Logic */

void		*routine(void *arg);
int			do_coder_actions(t_thread_data *coder, t_data *data,
				int left, int right);
int			do_rest_of_routine(t_thread_data *coder, t_data *data,
				int left, int right);

/* Monitor Logic */

void		*monitor(void *arg);

/* Data Setup & Memory */

int			data_setup(t_data **data, char **argv);
void		free_data(t_data **data);

#endif
