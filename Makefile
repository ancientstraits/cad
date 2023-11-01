LIBS = cglm epoxy glfw3

CFLAGS = -g -ggdb -Iinclude -Wall -Werror=vla $(shell pkg-config --cflags $(LIBS))
LFLAGS = $(shell pkg-config --libs $(LIBS)) -lm

OS = $(shell uname)
ifeq ($(OS),Linux)
	LFLAGS += -lEGL -lGL
else
	ifeq ($(OS),Darwin)
		LFLAGS += -framework OpenGL
	else
# windows
		LFLAGS += -lopengl32 
	endif
endif

OBJS := $(patsubst  src/%.c, obj/%.o, $(wildcard src/*.c))
DEPS := $(wildcard include/*.h)
CC = clang
EXEC = main

all: $(EXEC)

obj:
	[ -d obj ] || mkdir obj

obj/%.o: src/%.c $(DEPS) | obj
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)
