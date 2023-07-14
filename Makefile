NAME =	webserv

SRCS =	main.cpp \
		ErrorHandle.cpp WebsrvUtil.cpp \
		Config.cpp

OBJS =	$(SRCS:.cpp=.o)

CPP =	c++
CPPFLAGS = -Wall -Werror -Wextra -g -fsanitize=address -std=c++98# -fsanitize=address

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
	make all -j 8