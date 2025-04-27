srcdir = src
incdir = include
builddir = build
testdir = test

INCFLAGS = $(shell find $(incdir) -type d | sed 's/^/-I/' | tr '\n' ' ')
CFLAGS = $(INCFLAGS) -Wall -Wextra -std=gnu89 -fno-omit-frame-pointer
NASMFLAGS = -I$(incdir)/asm
LIB = $(builddir)/lib.so

DBUGFLAGS = -g
ifeq ($(DEBUG), 1)
	CFLAGS += $(DBUGFLAGS)
	NASMFLAGS += $(DBUGFLAGS)
	builddir = debug
endif

ifeq ($(OS), macos)
	CFLAGS += -arch x86_64
	NASMFLAGS += -f macho64 -D__MACH__	
	LIB = $(builddir)/lib.dylib
else
	CFLAGS += -z noexecstack -fPIC
	NASMFLAGS += -f elf64 -D__linux__
endif

srcs := $(shell find $(srcdir) -type f -name '*.c')
asm := $(shell find $(srcdir)/asm -type f -name '*.s')
tests := $(shell find $(testdir) -type f -name '*.c')

objs := $(patsubst $(srcdir)/%.c, $(builddir)/%.o, $(srcs))
asm_objs := $(patsubst $(srcdir)/asm/%.s, $(builddir)/asm/%.o, $(asm))
test_exes := $(patsubst $(testdir)/%.c, $(builddir)/$(testdir)/%, $(tests))

all : dirs $(objs) $(asm_objs) lib $(test_exes) asm

dirs : 
	@mkdir -p $(dir $(objs)) $(dir $(test_exes)) $(dir $(asm_objs)) 

$(builddir)/%.o : $(srcdir)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(builddir)/asm/%.o : $(srcdir)/asm/%.s
	nasm $(NASMFLAGS) -o $@ $<

lib :
	$(CC) $(CFLAGS) -shared -o $(LIB) $(objs) $(asm_objs)

$(builddir)/$(testdir)/% : $(testdir)/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LIB)

.PHONY : clean dirs lib asm debug

clean :
	-rm -r $(builddir)/*

