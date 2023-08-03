NAME		= webserv
CXX			= c++
CXXFLAGS	= -std=c++98 -Wall -Wextra -Werror
DEBUG		= -g

SRC			= main.cpp
SRCDIR		= src/

SRCS		= $(addprefix $(SRCDIR), $(SRC))
OBJS		= $(SRCS:.cpp=.o)
INC			= inc/

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUG) $(OBJS) -I$(INC) -o $(NAME)

clean	:
	@rm -rf $(OBJS)

fclean	: clean
	@rm -rf $(NAME)

re		: fclean all

.PHONY	: all clean fclean re

