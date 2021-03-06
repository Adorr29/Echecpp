##
## EPITECH PROJECT, 2018
## Echec++
## File description:
## Makefile
##

NAME		=	Echec++

CC		=	g++

SRCDIR		=	Src/
SRCPLANDIR	=	$(SRCDIR)Plan/
INCDIR		=	Include/

SRC		=	$(SRCDIR)Main.cpp \
			$(SRCDIR)Helper.cpp \
			$(SRCDIR)Receiver.cpp \
			$(SRCDIR)Player.cpp \
			$(SRCDIR)Plateau.cpp \
			$(SRCDIR)PawnRule.cpp \
			$(SRCPLANDIR)PlanPlateau.cpp \

OBJ		=	$(SRC:.cpp=.o)

CXXFLAGS	+=	-I $(INCDIR)
CXXFLAGS	+=	-W -Wall -Wextra

LDFLAGS		+=	-l sfml-system
LDFLAGS		+=	-l sfml-window
LDFLAGS		+=	-l sfml-graphics
LDFLAGS		+=	-l sfml-network

%.o		:	%.cpp
			@$(CC) -c -o $@ $< $(CXXFLAGS)

all		:	$(NAME)

$(NAME)		:	$(OBJ)
			@$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

clean		:
			@rm -f $(OBJ)

fclean		:	clean
			@rm -f $(NAME)

re		:	fclean all

debug		:	CXXFLAGS += -g3
debug		:	re
