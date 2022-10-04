# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/27 13:15:29 by tnaton            #+#    #+#              #
#    Updated: 2022/10/04 15:02:15 by nsartral         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

vpath %.cpp srcs
vpath %.cpp srcs/config
vpath %.hpp srcs
vpath %.hpp srcs/config

NAME = webserv

OBJDIR := objs

SRCS = webserv.cpp Request.cpp Nico.cpp Response.cpp Client.cpp server.cpp scope_configuration.cpp scope_http.cpp scope_server.cpp scope_location.cpp

INC = server.hpp Request.hpp Client.hpp Response.hpp scope_configuration.hpp scope_http.hpp scope_server.hpp scope_location.hpp

CPPFLAGS = -Wall -Wextra -Werror -g -std=c++98 #-fsanitize=address

CXX = c++

objects := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

$(NAME) : $(objects) $(INC)
	$(CXX) $(CPPFLAGS) $(objects) -o $@

$(objects): $(INC)

$(objects) : | $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
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
