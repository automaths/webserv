# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nsartral <nsartral@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/27 13:15:29 by tnaton            #+#    #+#              #
<<<<<<< HEAD
#    Updated: 2022/10/18 14:43:02 by nsartral         ###   ########.fr        #
=======
#    Updated: 2022/10/18 11:33:02 by nsartral         ###   ########.fr        #
>>>>>>> ebdf1f09fc7b01da6b17e2c603779a65682424be
#                                                                              #
# **************************************************************************** #

vpath %.cpp srcs
vpath %.cpp srcs/config
vpath %.hpp srcs
vpath %.hpp srcs/config

NAME = webserv

OBJDIR := objs

SRCS = webserv.cpp Request.cpp Response.cpp Client.cpp server.cpp scope_configuration.cpp scope_http.cpp scope_server.cpp scope_location.cpp ListDirectory.cpp cgi.cpp StatusCode.cpp

INC = server.hpp Request.hpp Client.hpp Response.hpp scope_configuration.hpp scope_http.hpp scope_server.hpp scope_location.hpp StatusCode.hpp

CXXFLAGS = -Wall -Wextra -Werror -Wpedantic -g -std=c++98 -o3

CXX = c++

objects := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

$(NAME) : $(objects) $(INC)
	$(CXX) $(CXXFLAGS) $(objects) -o $@

$(objects): $(INC)

$(objects) : | $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

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
