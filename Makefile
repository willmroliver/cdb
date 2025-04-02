srcdir = src
incdir = include
builddir = build
testdir = test

INCFLAGS = $(shell find $(incdir) -type d | sed 's/^/-I/' | tr '\n' ' ')
CFLAGS = $(INCFLAGS) -Wall -Wextra

DBUGFLAGS = -g
ifeq ($(DEBUG), 1)
	CFLAGS += $(DBUGFLAGS)
	builddir = debug
endif

srcs := $(shell find $(srcdir) -type f -name '*.c')
tests := $(wildcard $(testdir)/*.c)

objs := $(patsubst $(srcdir)/%.c, $(builddir)/%.o, $(srcs))
test_exes := $(patsubst $(testdir)/%.c, $(builddir)/$(testdir)/%, $(tests))

all : dirs $(objs) $(test_exes)
	
dirs : 
	@mkdir -p $(dir $(objs)) $(dir $(test_exes)) 

$(builddir)/%.o : $(srcdir)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
$(builddir)/$(testdir)/% : $(testdir)/%.c $(objs) 
	$(CC) $(CFLAGS) -o $@ $< $(objs) 

.PHONY : clean dirs debug

clean :
	-rm -r $(builddir)/*
