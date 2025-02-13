###############################################################################
###
### MIT License
###
### Copyright(c) 2025 TekyoDrift
###
### Permission is hereby granted, free of charge, to any person obtaining a
### copy of this software and associated documentation files (the "Software"),
### to deal in the Software without restriction, including without limitation
### the rights to use, copy, modify, merge, publish, distribute, sublicense,
### and/or sell copies of the Software, and to permit persons to whom the
### Software is furnished to do so, subject to the following coditions:
###
### The above copyright notice and this permission notice shall be included
### in all copies or substantial portions of the Software?
###
### THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
### IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
### FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
### AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
### LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
### FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
### DEALINGS IN THE SOFTWARE.
###
###############################################################################

###############################################################################
## Configuration
###############################################################################

CXX					=	g++

CXXFLAGS			=	-std=c++20 \
						-Isource \
						-Wall \
						-Wextra \
						-pthread \
						-lsfml-system \
						-lsfml-graphics \
						-lsfml-window \
						-lsfml-audio \
						-lz \
						-lGL \
						-lm

TARGET				=	neon-abyss
SERVER_TARGET		=	neon-abyss-server
PACKER_TARGET		=	neon-abyss-packer

###############################################################################
## Metadata
###############################################################################

AUTHOR				=	mallory-scotton
DATE				=	
HASH				=	

###############################################################################
## Sources
###############################################################################

SOURCE_DIRECTORY	=	source

SOURCES				=	$(shell find $(SOURCE_DIRECTORY) -name '*.cpp')

SERVER_SOURCES		=	source/utils/Args.cpp \
						source/network/Server.cpp \
						source/network/ServerDiscovery.cpp \
						source/network/Packet.cpp \
						source/network/Network.cpp \
						source/Main.cpp

PACKER_SOURCES		=	source/resources/AssetsPacker.cpp \
						source/resources/Compressor.cpp \
						source/Main.cpp

###############################################################################
## Makefile logic
###############################################################################

ifeq ($(shell git rev-parse HEAD > /dev/null; echo $$?),0)
	AUTHOR			:=	$(shell git log --format='%aN' | sort -u | awk \
						'{printf "%s, ", $$0}' | rev | cut -c 3- | rev)
	DATE			:=	$(shell git log -1 --date=format:"%Y/%m/%d %T" \
						--format="%ad")
	HASH			:=	$(shell git rev-parse --short HEAD)
endif

MFLAGS				:=	$(CXXFLAGS)

# Constant for colors
COM_COLOR			=	\033[0;34m
OBJ_COLOR			=	\033[0;36m
OK_COLOR			=	\033[0;32m
ERROR_COLOR			=	\033[0;31m
WARN_COLOR			=	\033[0;33m
NO_COLOR			=	\033[m

OBJECTS				=	$(SOURCES:.cpp=.o)
SERVER_OBJECTS		=	$(SERVER_SOURCES:.cpp=.o)
PACKER_OBJECTS		=	$(PACKER_SOURCES:.cpp=.o)

DEPENDENCIES		=	$(SOURCES:.cpp=.d)

###############################################################################
## Makefile rules
###############################################################################

all:
	make build
	make server
	make packer

-include $(DEPENDENCIES)

%.o: %.cpp
	@./scripts/progress.sh
	@./scripts/run.sh "$(CXX) -c $< -o $@ $(CXXFLAGS)" "$@"

clear:
	@rm -f source/Main.o

header:
	@printf "%b" "$(OK_COLOR)"
	@cat .art
	@echo
ifneq ($(HASH),)
	@printf "%b" "$(OBJ_COLOR)Name:	$(WARN_COLOR)$(TARGET)@$(HASH)\n"
else
	@printf "%b" "$(OBJ_COLOR)Name:	$(WARN_COLOR)$(TARGET)\n"
endif
	@printf "%b" "$(OBJ_COLOR)Author:	$(WARN_COLOR)$(AUTHOR)\n"
	@printf "%b" "$(OBJ_COLOR)Date: 	$(WARN_COLOR)$(DATE)\n\033[m"
	@printf "%b" "$(OBJ_COLOR)CC: 	$(WARN_COLOR)$(CXX)\n\033[m"
	@printf "%b" "$(OBJ_COLOR)Flags: 	$(WARN_COLOR)$(MFLAGS)\n\033[m"
	@echo

setup: header
	@./scripts/setup.sh "$(SOURCES)" "$(EXTENSION)"

build: CXXFLAGS += -MMD -MF $(@:.o=.d)
build: setup clear $(OBJECTS)
	@./scripts/run.sh "$(CXX) -o $(TARGET) $(OBJECTS) $(CXXFLAGS)" "$@"
	@rm -f .build

debug: CXXFLAGS += -g3
debug: build

server: TARGET = $(SERVER_TARGET)
server: OBJECTS = $(SERVER_OBJECTS)
server: CXXFLAGS += -DNEON_SERVER
server: clear build

packer: TARGET = $(PACKER_TARGET)
packer: OBJECTS = $(PACKER_OBJECTS)
packer: CXXFLAGS += -DNEON_PACKER
packer: clear build

clean:
	@find . -type f -iname "*.o" -delete
	@find . -type f -iname "*.d" -delete
	@find . -type f -iname "*.gcda" -delete
	@find . -type f -iname "*.gcno" -delete
	@find . -type f -iname "*.html" -delete
	@find . -type f -iname "*.css" -delete

fclean: clean
	@rm -f $(TARGET)
	@rm -f $(SERVER_TARGET)

re: fclean build
res: fclean server

.PHONY: all build debug server clean fclean re res
