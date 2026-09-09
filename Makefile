CC        := cc
CFLAGS    := -Wall -Wextra -Werror -pthread -g3 -I.
NAME      := codexion

SRCS      := codexion.c \
             utils_2.c \
             monitor.c \
             routine.c \
			 utils_1.c \
             init.c \
             scheduler.c

OBJS      := $(SRCS:.c=.o)

HEADER    := codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
