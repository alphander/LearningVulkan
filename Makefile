# [Settings]
SRC:=src
LIB:=lib
INCLUDE:=include
OBJ:=obj

CC:=clang
RM:=del
OUT:=a.exe

INCLUDES:=-I./$(INCLUDE) -I./$(SRC) -I"C:/Program Files/VulkanSDK/1.3.243.0/Include"
LIBS:=-L./$(LIB) -L"C:/Program Files/VulkanSDK/1.3.243.0/Lib"

LIBFILES:=$(wildcard $(LIB)/*.a $(LIB)/**/*.a $(LIB)/**/**/*.a)

DEFINES:= -D_BSD_SOURCE

LDFLAGS:= -lgdi32 -lvulkan -ldl -lpthread

CFLAGS_ND:=-Wall -O2 -std=c17 -DNDEBUG
CFLAGS_D:=-Wall -g -O0 -std=c17 -DDEBUG

# [Debug and Release rules]

.PHONY: debug release clean build all

all: debug

release: CFLAGS=$(CFLAGS_ND)
release: build

debug: CFLAGS=$(CFLAGS_D)
debug: build

build: $(OUT)

# [Compile C program]

ASSEMBLE=$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(LIBFILES) $(LDFLAGS)
COMPILE=$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) $(DEFINES)

$(OUT):\
	$(OBJ)/main.o\
	$(OBJ)/logging.o
	$(ASSEMBLE)

$(OBJ)/main.o:\
	$(SRC)/main.c\
	$(SRC)/logging/logging.h
	$(COMPILE)

$(OBJ)/logging.o:\
	$(SRC)/logging/logging.c
	$(COMPILE)

# [Clean C program]

clean:
	$(RM) $(OUT)
	$(RM) /q $(OBJ)