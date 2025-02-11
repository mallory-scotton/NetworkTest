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
						-lm

TARGET				=	neon-abyss
SERVER_TARGET		=	neon-abyss-server

###############################################################################
## Sources
###############################################################################

SOURCE_DIRECTORY	=	source
SOURCES				=	$(shell find $(SOURCE_DIRECTORY) -name '*.cpp')
OBJECTS				=	$(SOURCES:.cpp=.o)

###############################################################################
## Makefile logic
###############################################################################

all:
	make fclean
	make build
	make clean
	make server

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

build: $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(CXXFLAGS)

debug: CXXFLAGS += -g3
debug: build

server: TARGET = $(SERVER_TARGET)
server: CXXFLAGS += -DNEON_SERVER
server: build

clean:
	find . -type f -iname "*.o" -delete
	find . -type f -iname "*.d" -delete
	find . -type f -iname "*.gcda" -delete
	find . -type f -iname "*.gcno" -delete
	find . -type f -iname "*.html" -delete
	find . -type f -iname "*.css" -delete

fclean: clean
	rm -f $(TARGET)
	rm -f $(SERVER_TARGET)

re: fclean build
res: fclean server

.PHONY: all build debug server clean fclean re res
