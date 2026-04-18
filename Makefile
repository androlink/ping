NAME = ft_ping
ARGS =
CC = cc
RMF = rm -f

DEBUGFLAGS = -g -fsanitize=address

CFLAGS = -g -Wall -Wextra -Werror $(DEBUGFLAGS)
DFLAGS = -MP -MMD


SDIR = srcs
BDIR = .build
DDIR = $(BDIR)
HDIR = includes/

SRCS :=


SFILES = $(SRCS:%=$(SDIR)/%)

DFILES = $(SRCS:%.c=$(DDIR)/%.d)

OFILES = $(SRCS:%.c=$(BDIR)/%.o)

LIB_FLAGS := -l termcap

all:
	@echo "compiling $(NAME):"
	@$(MAKE) -s $(NAME)

include config/srcs.mk

$(NAME) : $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES) $(LIB_FLAGS) 
	@echo "$(NAME) compilation done";

-include $(DFILES)

$(BDIR)/%.o : $(SDIR)/%.c Makefile

	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@ -I $(HDIR) 
	@echo "	$@"

clean	::
	@echo "cleaning $(NAME)";
	@$(RMF) $(OFILES) $(DFILES)

re		::	fclean
	@$(MAKE) -s all

fclean	::	clean
	@$(RMF) $(NAME)

force :

run: all
	@echo "run __$(NAME)__"
	@./$(NAME) $(ARGS)

valgrind: all
	@echo "valgrind __$(NAME)__"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(NAME) $(ARGS)


.PHONY: clean re fclean force all norm run valgrind
