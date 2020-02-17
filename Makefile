CC = g++
CFLAGS = -Wall -Wextra -Werror -g

NAME = Matt_daemon
FILES = main.cpp cred_daemon.cpp \
	lock_daemon.cpp srv_daemon.cpp Reporter.cpp \
	Error_daemon.cpp

SRCS_DIR = srcs
SRCS = $(addprefix $(SRCS_DIR)/,$(FILES))

INCS_DIR = includes
INCS = -I $(INCS_DIR)

OBJS_DIR = objs
OBJS = $(addprefix $(OBJS_DIR)/,$(FILES:%.cpp=%.o))

.PHONY: all
all: $(NAME)

$(NAME): $(OBJS_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(INCS) -o $(NAME) $(OBJS)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf objs

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all
