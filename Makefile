src = $(wildcard src/*.c)
obj = $(src:src/%.c=obj/%.o)
objs = $(src:src/%.c=obj/%.oo)

hdir = $(shell pwd)/hdr
odir = $(shell pwd)/obj
ldir = $(shell pwd)/lib

libname = $(ldir)/libflowtable.a
libshared = $(ldir)/libflowtable.so

AR = ar
CC = gcc
SHAREDCP = -fPIC
SHAREDLD = -shared
LNAME = -lflowtable

DEBUG = -ggdb -g

MAIN = main.c
export LD_LIBRARY_PATH=$(ldir)

$(libname): $(obj)
	$(AR) rc $@ $^
	ranlib $(libname)

#$(obj): | objd

#objd:
#	@mkdir -p obj

#obj/%.o : %.c
#	@echo $<
#	$(CC) -c -I$(hdir)  $< -o $@

$(obj): | odir
	$(CC) -c $(src) -I$(hdir) -o $@

odir:
	mkdir -p $(odir)
	mkdir -p $(ldir)

$(objs):
	$(CC) $(DEBUG) -c $(SHAREDCP) $(src) -I$(hdir) -o $@

shared: $(objs)
	$(CC) $(DEBUG) $(SHAREDLD) -o $(libshared) $(objs)

clean:
	rm -f $(ldir)/* $(odir)/* example

example: $(obj) shared
	$(CC) $(DEBUG) $(MAIN) -L$(ldir) -I$(hdir) $(LNAME) -o $@

run: example
	./$< $(LNAME)
