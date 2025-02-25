CPP_COMPILER = g++
C_COMPILER = gcc
SRC_DIRS = src

INCLUDE_FOLDER = -Idependencies
LIB_FOLDER = -Ldependencies/lib
LINKER_FLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

FLAGS = -Wall -Wextra -O2 -MP -MD $(INCLUDE_FOLDER) $(LIB_FOLDER) $(LINKER_FLAGS)

SRC_FILES = $(foreach D, $(SRC_DIRS), $(wildcard $(D)/*.c $(D)/*.cpp))
OBJECTS = $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SRC_FILES)))
DEPENDENCIES = $(patsubst %.o, %.d, $(OBJECTS))

-include $(DEPENDENCIES)

# target: requirements
# $^: inserts the requirements of the rule
# $<: inserts the dependencies of the rule requirements
# $@: inserts the target

%.o: %.c
	$(C_COMPILER) $(FLAGS) $< -c -o $@

%.o: %.cpp
	$(CPP_COMPILER) $(FLAGS) $< -c -o $@

project: $(OBJECTS)
	$(CPP_COMPILER) $(FLAGS) $^ -o $@

all: project
	./project

clean:
	rm -rf $(OBJECTS) $(DEPENDENCIES)
