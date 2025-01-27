CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/inc
OBJ_DIR := $(CUR_DIR)/obj
OBJ_STATIC_DIR := $(OBJ_DIR)/static
OBJ_SHARED_DIR := $(OBJ_DIR)/shared
SRC_DIR := $(CUR_DIR)/src
LIB_DIR := $(CUR_DIR)/lib
LIB_STATIC_DIR := $(LIB_DIR)/static
LIB_SHARED_DIR := $(LIB_DIR)/shared

C_FLAGS = -I $(INC_DIR)

CREATE_OBJS:
	gcc -c       $(SRC_DIR)/hello.c -o $(OBJ_STATIC_DIR)/hello.o
	gcc -c -fPIC $(SRC_DIR)/hello.c -o $(OBJ_SHARED_DIR)/hello.o
	gcc -c  $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

CREATE_LIB:
	ar rcs $(LIB_STATIC_DIR)/libhello.a $(OBJ_STATIC_DIR)/hello.o
	gcc -shared $(OBJ_SHARED_DIR)/hello.o -o $(LIB_SHARED_DIR)/libhello.so

all: CREATE_OBJS CREATE_LIB
	gcc  $(OBJ_DIR)/main.o -L$(LIB_STATIC_DIR) -lhello -o $(BIN_DIR)/statically-linked
	gcc  $(OBJ_DIR)/main.o -L$(LIB_SHARED_DIR) -lhello -o $(BIN_DIR)/use-shared-library

clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(OBJ_STATIC_DIR)/*.o
	rm -rf $(OBJ_SHARED_DIR)/*.o
	rm -rf $(LIB_SHARED_DIR)/*.so
	rm -rf $(LIB_STATIC_DIR)/*.a
	rm -rf $(BIN_DIR)/*

