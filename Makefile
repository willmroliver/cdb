srcdir = src
incdir = include
builddir = build
testdir = test

CFLAGS = -I$(incdir) -Wall -Wextra

DBUGFLAGS = -g
ifeq ($(DEBUG), 1)
	CFLAGS += $(DBUGFLAGS)
	builddir = debug
endif

srcs := $(wildcard $(srcdir)/*.c)
tests := $(wildcard $(testdir)/*.c)

objs := $(patsubst $(srcdir)/%.c, $(builddir)/%.o, $(srcs))
test_exes := $(patsubst $(testdir)/%.c, $(builddir)/$(testdir)/%, $(tests))

all : dirs $(objs) $(test_exes)
	
dirs : 
	@mkdir -p $(builddir)/$(testdir)

$(builddir)/%.o : $(srcdir)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
$(builddir)/$(testdir)/% : $(testdir)/%.c $(objs) 
	$(CC) $(CFLAGS) -o $@ $< $(objs) 

.PHONY : clean dirs debug

clean :
	-rm -r $(builddir)/*
