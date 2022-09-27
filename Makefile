# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tnaton <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/27 13:15:29 by tnaton            #+#    #+#              #
#    Updated: 2022/09/27 18:31:01 by bdetune          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

OBJDIR := objs

SRCS = server.cpp Client.cpp

INC = server.hpp

source = $(addprefix srcs/,$(SRCS))

include = $(addprefix srcs/,$(INC))

CPPFLAGS = -Wall -Wextra -Werror -g -std=c++98

CXX = c++

objects := $(patsubst srcs/%.cpp,$(OBJDIR)/%.o,$(source))

$(NAME) : $(objects) $(include)
	$(CXX) $(CPPFLAGS) $(objects) -o $@

$(objects): $(include)

$(objects) : | $(OBJDIR)

$(OBJDIR)/%.o: srcs/%.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

$(OBJDIR) :
	mkdir $(OBJDIR)

.PHONY: all
all : $(NAME)

.PHONY: clean
clean :
	rm -rf $(objects) $(OBJDIR)

.PHONY: fclean
fclean:
	rm -rf $(NAME) $(objects) $(OBJDIR)

.PHONY: re
re : fclean all
