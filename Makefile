# ---------------------------------------------------------------------------- #
#   Define the target                                                          #
# ---------------------------------------------------------------------------- #

NAME				:=	webserv

# ---------------------------------------------------------------------------- #
#   Define the compiler and flags                                              #
# ---------------------------------------------------------------------------- #

CPP					=	c++
CPPFLAGS			=	-Wall -Werror -Wextra -std=c++98 -g -fsanitize=address
# CPPFLAGS			=	-Wall -Werror -Wextra -std=c++98
DEPFLAGS			=	-MMD -MP -MF $(BUILD_DIR)/$(DEP_DIR)/$*.d

# ---------------------------------------------------------------------------- #
#   Define the directories                                                     #
# ---------------------------------------------------------------------------- #

SRC_DIR				:=	srcs
CONFIG_DIR			:=	config
REQUEST_DIR			:=	request
SOCKET_DIR			:=	socket
UTIL_DIR			:=	util
BUILD_DIR			:=	build
OBJ_DIR				:=	objs
DEP_DIR				:=	deps

# ---------------------------------------------------------------------------- #
#   Define the source files                                                    #
# ---------------------------------------------------------------------------- #

SRCS				:=	$(addprefix $(SRC_DIR)/, main.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/, SpiderMen.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(CONFIG_DIR)/, Config.cpp Location.cpp Server.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(REQUEST_DIR)/, ARequest.cpp RDelete.cpp RGet.cpp RPost.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(SOCKET_DIR)/, Socket.cpp Client.cpp)
SRCS				+=	$(addprefix $(SRC_DIR)/$(UTIL_DIR)/, SpiderMenUtil.cpp Time.cpp)
OBJS				:=	$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(OBJ_DIR)/%.o, $(SRCS))
DEPS				:=	$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(DEP_DIR)/%.d, $(SRCS))

# ---------------------------------------------------------------------------- #
#   Define the rules                                                           #
# ---------------------------------------------------------------------------- #

all : $(NAME)

$(NAME) : $(OBJS)
	@$(CPP) $(CPPFLAGS) $^ -o $@
	@printf "\n[$(NAME)] Linking Success\n"

$(BUILD_DIR)/$(OBJ_DIR)%.o : $(SRC_DIR)/%.cpp | dir_guard
	@$(CPP) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	@$(RM) -rf $(BUILD_DIR)

fclean:	clean
	@$(RM) -rf $(NAME)

re : fclean
	@$(MAKE)

dir_guard :
	@mkdir -p $(addprefix $(BUILD_DIR)/$(OBJ_DIR)/, $(SRC_DIR) \
	$(CONFIG_DIR) $(REQUEST_DIR) $(SOCKET_DIR) $(UTIL_DIR))
	@mkdir -p $(addprefix $(BUILD_DIR)/$(DEP_DIR)/, $(SRC_DIR) \
	$(CONFIG_DIR) $(REQUEST_DIR) $(SOCKET_DIR) $(UTIL_DIR))

.PHONY: all clean fclean re dir_guard

-include $(DEPS)