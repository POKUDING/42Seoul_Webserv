NAME =	webserv

SRCS =	main.cpp \
		config.cpp

OBJS =	$(SRCS:.cpp=.o)

CPP =	c++
CPPFLAGS = -Wall -Werror -Wextra -std=c++98# -fsanitize=address

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $@

%.o:	%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

.PHONY: clean fclean re all
clean:
	rm -rf $(OBJS)

fclean:	clean
	rm -rf $(NAME)

re:
	make fclean
	make all