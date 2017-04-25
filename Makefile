src = $(wildcard src/*.c)
obj = $(src:src/%.c=obj/%.o)
objs = $(src:src/%.c=obj/%.oo)

hdir = $(shell pwd)/hdr
odir = $(shell pwd)/obj
ldir = $(shell pwd)/lib

libname = libflowtable
libstatic = $(ldir)/$(libname).a
libshared = $(ldir)/$(libname).so

INCLUDEDIR = /usr/local/include
LIBDIR = /usr/local/lib

AR = ar
CC = gcc
SHAREDCP = -fPIC
SHAREDLD = -shared
LNAME = -lflowtable

CFLAGS = -std=c99
DEBUG = -ggdb -g

MAIN = main.c
export LD_LIBRARY_PATH=$(ldir)

$(libstatic): $(obj)
	$(AR) rc $@ $^
	ranlib $(libstatic)

#$(obj): | objd

#objd:
#	@mkdir -p obj

#obj/%.o : %.c
#	@echo $<
#	$(CC) -c -I$(hdir)  $< -o $@

$(obj): | odir
	$(CC) $(CFLAGS) -c $(src) -I$(hdir) -o $@

odir:
	mkdir -p $(odir)
	mkdir -p $(ldir)

$(objs):
	$(CC) $(CFLAGS) $(DEBUG) -c $(SHAREDCP) $(src) -I$(hdir) -o $@

shared: $(objs)
	$(CC) $(CFLAGS) $(DEBUG) $(SHAREDLD) -o $(libshared) $(objs)

clean:
	rm -f $(ldir)/* $(odir)/* example

example: $(obj) shared
	$(CC) $(CFLAGS) $(DEBUG) $(MAIN) -L$(ldir) -I$(hdir) $(LNAME) -o $@

run: example
	$(shell export LD_LIBRARY_PATH=$(ldir))
	./$< $(LNAME)

install: shared
	cp $(hdir)/flow_table.h $(INCLUDEDIR)/
	cp $(ldir)/$(libname).so $(LIBDIR)/

remove:
	rm -f $(INCLUDEDIR)/flow_table.h
	rm -f $(LIBDIR)/$(libname).so
