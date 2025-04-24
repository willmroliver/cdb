srcdir = src
incdir = include
builddir = build
testdir = test

INCFLAGS = $(shell find $(incdir) -type d | sed 's/^/-I/' | tr '\n' ' ')
CFLAGS = $(INCFLAGS) -Wall -Wextra -std=gnu89

DBUGFLAGS = -g
ifeq ($(DEBUG), 1)
	CFLAGS += $(DBUGFLAGS)
	builddir = debug
endif

NASMFLAGS = -f elf64
ifeq ($(OS), macos)
	NASMFLAGS = -f macho64
	CFLAGS += -arch x86_64
endif

srcs := $(shell find $(srcdir) -type f -name '*.c')
asm := $(shell find $(srcdir)/asm -type f -name '*.s')
tests := $(shell find $(testdir) -type f -name '*.c')

objs := $(patsubst $(srcdir)/%.c, $(builddir)/%.o, $(srcs))
asm_objs := $(patsubst $(srcdir)/asm/%.s, $(builddir)/asm/%.o, $(asm))
test_exes := $(patsubst $(testdir)/%.c, $(builddir)/$(testdir)/%, $(tests))

all : dirs $(objs) $(asm_objs) $(test_exes) asm
	
dirs : 
	@mkdir -p $(dir $(objs)) $(dir $(test_exes)) $(dir $(asm_objs)) 

$(builddir)/%.o : $(srcdir)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(builddir)/asm/%.o : $(srcdir)/asm/%.s
	nasm $(NASMFLAGS) -o $@ $<
	
$(builddir)/$(testdir)/% : $(testdir)/%.c $(objs) $(asm_objs) 
	$(CC) $(CFLAGS) -o $@ $< $(objs) $(asm_objs)

.PHONY : clean dirs asm debug

clean :
	-rm -r $(builddir)/*
