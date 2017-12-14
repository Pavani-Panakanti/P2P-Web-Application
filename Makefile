.SILENT:

MODULE := NODE

CC := gcc
CXX := g++
AR := ar
INC := ./inc/

CFLAGS := -g -c -I $(INC)
//EFLAGS := -fno-stack-protector -lpthread -fno-common -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include
EFLAGS := -fno-stack-protector -lpthread -fno-common -lcrypto

BASE_DIR = .

SRC_DIR = ./src

OBJ_DIR = ./obj

OBJS = NodeMain.o\
       NodeCommon.o\
       NodeServ.o\
       NodeClnt.o\
       NodeCli.o\
       NodeApi.o\
       NodeHttp.o\
       Node.o

all : $(OBJ_DIR)/NodeMain.o\
      $(OBJ_DIR)/NodeCommon.o\
      $(OBJ_DIR)/NodeServ.o\
      $(OBJ_DIR)/NodeClnt.o\
      $(OBJ_DIR)/NodeCli.o\
      $(OBJ_DIR)/NodeApi.o\
      $(OBJ_DIR)/NodeHttp.o\
      $(BASE_DIR)/Node.o

$(OBJ_DIR)/NodeMain.o : $(SRC_DIR)/NodeMain.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) $< -o $@ $(EFLAGS)

$(OBJ_DIR)/NodeCommon.o : $(SRC_DIR)/NodeCommon.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(EFLAGS)

$(OBJ_DIR)/NodeServ.o : $(SRC_DIR)/NodeServ.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(EFLAGS)

$(OBJ_DIR)/NodeClnt.o : $(SRC_DIR)/NodeClnt.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(EFLAGS)

$(OBJ_DIR)/NodeCli.o : $(SRC_DIR)/NodeCli.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(EFLAGS)

$(OBJ_DIR)/NodeApi.o : $(SRC_DIR)/NodeApi.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(EFLAGS)

$(OBJ_DIR)/NodeHttp.o : $(SRC_DIR)/NodeHttp.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(EFLAGS)

$(BASE_DIR)/Node.o :
	$(CXX) -g\
        $(OBJ_DIR)/NodeMain.o\
        $(OBJ_DIR)/NodeServ.o\
        $(OBJ_DIR)/NodeClnt.o\
        $(OBJ_DIR)/NodeCommon.o\
        $(OBJ_DIR)/NodeCli.o\
        $(OBJ_DIR)/NodeApi.o\
        $(OBJ_DIR)/NodeHttp.o\
        -o $(BASE_DIR)/Node.o $(EFLAGS)

clean :
	rm -rf $(OBJ_DIR)
	rm $(BASE_DIR)/Node.o
