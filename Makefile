# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hirwatan <hirwatan@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/02 11:44:47 by hirwatan          #+#    #+#              #
#    Updated: 2025/08/02 12:15:20 by hirwatan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:= ircserv
CXX			:= c++
CFLAGS		:= -Wall -Wextra -Werror -std=c++98 -pedantic
RM			:= rm -rf

# Project PATH
ROOT_DIR	:= .
SRC_DIR		:= $(ROOT_DIR)/srcs
INC_DIR		:= $(ROOT_DIR)/includes
OBJ_DIR		:= $(ROOT_DIR)/objs

# FLAGS
IDFLAGS		:= -I$(INC_DIR)

SRC		:= $(shell find $(SRC_DIR) -name '*.cpp')
OBJ		:= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

# COLOR
COLOUR_GREEN=\033[0;32m
COLOUR_RED=\033[0;31m
COLOUR_BLUE=\033[0;34m
COLOUR_END=\033[0m

# =======================
# == Targets ============
# =======================
all:
	$(MAKE) __build -j $(shell nproc)

# =======================
# ==== Build Targets ====
# =======================
__build: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CFLAGS) $(OBJ) $(IDFLAGS) -o $(NAME)
	@if [ $$? -eq 0 ]; then \
		echo "$(COLOUR_GREEN)😀Build Complete😀$(COLOUR_END)"; \
	fi

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(IDFLAGS) -c $< -o $@

# =======================
# == Clean Targets ======
# =======================
clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re