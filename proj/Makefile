# This is a not-so-simple Makefile

# Define what compiler to use and the flags.
CC=cc
CXX=g++
CCFLAGS= -g -std=c++23 -Wall -Werror
CXXFLAGS = -Iinclude

TARGET = proj

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=%.o)

all: $(TARGET)

%.o : $(SRC_DIR)/%.cpp
	$(CXX) -c $(CCFLAGS) $(CXXFLAGS) $^

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ)

clean:
	rm -f *.o proj
