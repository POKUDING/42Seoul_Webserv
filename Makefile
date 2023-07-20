# ---------------------------------------------------------------------------- #
#   Define the target                                                          #
# ---------------------------------------------------------------------------- #

NAME				:=	webserv

# ---------------------------------------------------------------------------- #
#   Define the compiler and flags                                              #
# ---------------------------------------------------------------------------- #

CPP					=	c++
CPPFLAGS			=	-Wall -Werror -Wextra -std=c++98# -g -fsanitize=address
DEPFLAGS			=	-MMD -MP -MF $(BUILD_DIR)/$(DEP_DIR)/$*.d

# ---------------------------------------------------------------------------- #
#   Define the directories                                                     #
# ---------------------------------------------------------------------------- #

SRC_DIR				:=	srcs
CONFIG_DIR			:=	config
REQUEST_DIR			:=	request
SOCKET_DIR			:=	socket
UTIL_DIR			:=	util

# ---------------------------------------------------------------------------- #
#   Define the source files                                                    #
# ---------------------------------------------------------------------------- #

SRCS				:=	main.cpp
SRCS				+=	$(addprefix $(SRC_DIR)/, SpiderMen.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(CONFIG_DIR)/, Config.cpp Location.cpp Server.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(REQUEST_DIR)/, ARequest.cpp RDelete.cpp RGet.cpp RPost.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(SOCKET_DIR)/, ASocket.cpp Client.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(UTIL_DIR)/, SpiderMenUtil.cpp)
OBJS				:=	$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(OBJ_DIR)/%.o, $(SRCS))
DEPS				:=	$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(DEP_DIR)/%.d, $(SRCS))

all : $(NAME)

$(NAME) : $(OBJS)
	@$(CPP) $(CPPFLAGS) $(LDFLAGS) $(OBJS) -o $@
	@printf "\n[$(NAME)] Linking S"

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