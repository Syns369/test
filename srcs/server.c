/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdarcour <jdarcour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/26 15:40:50 by jdarcour          #+#    #+#             */
/*   Updated: 2023/09/29 15:37:37 by jdarcour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minitalk.h"

void	handle_sigint(char *buffer)
{
	ft_printf("Received SIGINT\n");
	if (buffer)
		free(buffer);
	exit(0);
}

void	handle_length(t_data *data, int signum)
{
	if (data->bit_count < 31)
	{
		data->len <<= 1;
		data->len |= (signum == SIGUSR1);
		(data->bit_count)++;
	}
	else if (data->bit_count == 31)
	{
		data->len <<= 1;
		data->len |= (signum == SIGUSR1);
		ft_printf("Received length: %u\n", data->len);
		data->buffer = (char *)malloc(sizeof(char) * (data->len + 1));
		data->buffer[0] = '\0';
		(data->bit_count)++;
	}
}

void	handle_message(t_data *data, int signum)
{
	data->c |= (signum == SIGUSR1);
	data->i++;
	if (data->i == 8)
	{
		if (data->c == '\0')
		{
			ft_putstr_fd("Received message: ", 1);
			ft_putstr_fd(data->buffer, 1);
			ft_putchar_fd('\n', 1);
			free(data->buffer);
			data->buffer = NULL;
			data->bit_count = 0;
			data->len = 0;
			data->c = 0;
			data->i = 0;
		}
		else
		{
			ft_strlcat(data->buffer, (const char *) &data->c, data->len + 1);
			data->i = 0;
			data->c = 0;
		}
	}
	else
		data->c <<= 1;
}

void	handler(int signum)
{
	static t_data	data;

	if (signum == SIGINT)
		handle_sigint(data.buffer);
	if (data.bit_count < 32)
		handle_length(&data, signum);
	else
		handle_message(&data, signum);
}

int	main(void)
{
	struct sigaction	sa;

	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	ft_printf("Server PID: %d\n", getpid());
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	while (1)
		pause();
	return (0);
}
