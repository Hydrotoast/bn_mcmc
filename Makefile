SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
INC=lib/UnitTest
LIB=lib/UnitTest/Logger.a

CC=clang++
CFLAGS=-stdlib=libstdc++ -std=c++11 -c -I$(INC)
LFLAGS=$(LIB)

DEP=

TARGET=$(BIN_DIR)/bayes_net

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) -o$@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -o$@ $<

clean: $(OBJS)
	rm $(OBJS)
	
run: $(OBJ)
	./$(TARGET)

.PHONY:clean
