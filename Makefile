NAME =	webserv

CPP =	c++
CPPFLAGS = -Wall -Werror -Wextra -std=c++98# -g -fsanitize=address

# ---------------------------------------------------------------------------- #
#   Define the directories                                                     #
# ---------------------------------------------------------------------------- #

SRCS_DIR		:= srcs
CONFIG_DIR		:= config
REQUEST_DIR		:= request
SOCKET_DIR		:= socket
UTIL_DIR		:= util


# ---------------------------------------------------------------------------- #
#   Define the source files                                                    #
# ---------------------------------------------------------------------------- #

SRCS				:=	main.cpp
SRCS				+=	$(addprefix $(SRCS_DIR)/, SpiderMen.cpp)
SRCS				+=	$(addprefix $(SRCS_DIR)/$(CONFIG_DIR)/, Config.cpp Location.cpp Server.cpp)
SRCS				+=	$(addprefix $(SRCS_DIR)/$(REQUEST_DIR)/, ARequest.cpp RDelete.cpp RGet.cpp RPost.cpp)
SRCS				+=	$(addprefix $(SRCS_DIR)/$(SOCKET_DIR)/, ASocket.cpp Client.cpp)
SRCS				+=	$(addprefix $(SRCS_DIR)/$(UTIL_DIR)/, SpiderMenUtil.cpp)
# OBJS				:=	$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/$(OBJ_DIR)/%.o, $(SRCS))
# DEPS				:=	$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/$(DEP_DIR)/%.d, $(SRCS))
OBJS =	$(SRCS:.cpp=.o)

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $@

%.o:	%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean:	clean
	rm -rf $(NAME)

re:
	make fclean
	make all -j 8

dir_guard :
	@mkdir -p $(addprefix $(BUILD_DIR)/$(OBJ_DIR)/, $(SET_WINDOW_DIR) \
	$(CHECK_WINDOW_DIR) $(INIT_CUB3D_DIR) $(RUN_CUB3D_DIR) $(UTILS_DIR))

.PHONY: clean fclean re all dir_guard