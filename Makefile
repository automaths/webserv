# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tnaton <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/27 13:15:29 by tnaton            #+#    #+#              #
#    Updated: 2022/10/03 17:05:45 by bdetune          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

OBJDIR := objs

SRCS = webserv.cpp Request.cpp Response.cpp Client.cpp server.cpp config/scope_configuration.cpp config/scope_http.cpp config/scope_server.cpp config/scope_location.cpp

INC = server.hpp Request.hpp Client.hpp Response.hpp config/scope_configuration.hpp config/scope_http.hpp config/scope_server.hpp config/scope_location.hpp


source = $(addprefix srcs/,$(SRCS))

include = $(addprefix srcs/,$(INC))

CPPFLAGS = -Wall -Wextra -Werror -g -std=c++98 #-fsanitize=address

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
