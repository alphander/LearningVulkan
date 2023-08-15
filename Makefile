# [Settings]
SRC:=src
LIB:=lib
INCLUDE:=include
OBJ:=obj

CC:=clang
GLSLC=glslc
RM:=del
OUT:=a.exe

INCLUDES:=\
	-I./$(INCLUDE)\
	-I./$(SRC) -I"C:/Program Files/VulkanSDK/1.3.250.1/Include"

LIBS:=\
	-L./$(LIB)\
	-L"C:/Program Files/VulkanSDK/1.3.250.1/Lib" 

DEFINES:=

LDFLAGS:=-lvulkan-1

CFLAGS_ND:=-Wall -O2 -std=c17 -DNDEBUG
CFLAGS_D:=-Wall -g -O0 -std=c17 -DDEBUG 

GLSLFLAGS:=-std=460

# [Debug and Release rules]

.PHONY: debug release clean build all GLSL

all: debug

release: CFLAGS=$(CFLAGS_ND)
release: build

debug: CFLAGS=$(CFLAGS_D)
debug: build

build: $(OUT) GLSL


# [Compile program]

ASSEMBLE=$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(LDFLAGS)
COMPILE=$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) $(DEFINES)

# C Compile

$(OUT):\
	$(OBJ)/main.o\
	$(OBJ)/logging.o\
	$(OBJ)/util.o
	$(ASSEMBLE)

$(OBJ)/main.o:\
	$(SRC)/main.c\
	$(SRC)/util/logging.h\
	$(SRC)/util/util.h
	$(COMPILE)

$(OBJ)/logging.o:\
	$(SRC)/util/logging.c
	$(COMPILE)

$(OBJ)/util.o:\
	$(SRC)/util/util.c\
	$(SRC)/util/logging.h
	$(COMPILE)

# GLSL Compile 

COMPILE_GLSL=$(GLSLC) $(GLSLFLAGS) -c $< -o $@

GLSL:\
	$(OUT) $(OBJ)/computeshader.spv

$(OBJ)/computeshader.spv:\
	$(SRC)/shaders/computeshader.comp
	$(COMPILE_GLSL)

# [Clean program]

clean:
	$(RM) $(OUT)
	$(RM) /q $(OBJ)