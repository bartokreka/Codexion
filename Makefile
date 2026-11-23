# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rbartok <rbartok@student.42wolfsburg.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/02 18:07:48 by rbartok           #+#    #+#              #
#    Updated: 2026/09/23 19:02:45 by rbartok          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# NAME, all, clean, fclean, re

SRC = main.c arg_validation.c time_helpers.c thread_helpers.c simulation.c dongle_actions.c scheduler_helpers.c mutex_helpers.c heap_architecture.c heap_helpers.c
OBJ = $(SRC:.c=.o)
NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re