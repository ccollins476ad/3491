##### Compiler options.
CFLAGS := -g3 -O0 -std=gnu99
CFLAGS += -Wmissing-prototypes 
CFLAGS += -Waddress   
CFLAGS += -Warray-bounds
CFLAGS += -Wchar-subscripts  
CFLAGS += -Wenum-compare
CFLAGS += -Wimplicit-int
CFLAGS += -Wimplicit-function-declaration
CFLAGS += -Wcomment
CFLAGS += -Wformat
CFLAGS += -Wmain
CFLAGS += -Wmaybe-uninitialized
CFLAGS += -Wmissing-braces
CFLAGS += -Wnonnull
CFLAGS += -Wpointer-sign
CFLAGS += -Wreturn-type
CFLAGS += -Wsequence-point
#CFLAGS += -Wstrict-aliasing
CFLAGS += -Wstrict-overflow=1
CFLAGS += -Wswitch
CFLAGS += -Wtrigraphs
CFLAGS += -Wuninitialized
CFLAGS += -Wunknown-pragmas
CFLAGS += -Wunused-function
CFLAGS += -Wunused-label
CFLAGS += -Wunused-value
CFLAGS += -Wunused-variable
CFLAGS += -Wvolatile-register-var

ifeq ($(ALLOW_WARNINGS),)
    CFLAGS += -Werror
endif

#CFLAGS += -D TIXML_USE_STL
CFLAGS += -D ALLEGRO_STATICLINK

##### Chicken scheme options
SFLAGS += -O0 -d2

##### Linker options
LFLAGS := 
#LFLAGS += -verbose

##### Functions.

# $1: include directories
build_cflags = $(CFLAGS) $(foreach d, $(1), -iquote$d)

# $1: library directories
# $2: library names
build_lflags = $(LFLAGS) $(foreach d, $(1), -L$d) $(foreach f, $(2), -l$f)

ifeq ($(TARGET_OS),win32)
CFLAGS+=-iquote/home/ccollins/tmp/allegro-4.4.2-mingw-4.5.2/include
LFLAGS+=-L /home/ccollins/tmp/allegro-4.4.2-mingw-4.5.2/lib
LFLAGS+=-L /home/ccollins/tmp/dx80/lib
LFLAGS+=-L /home/ccollins/tmp/lib
CC=i686-pc-mingw32-gcc
AR=i686-pc-mingw32-ar

CFLAGS += -D__USE_MINGW_ANSI_STDIO
endif

SC=csc

ifdef VERBOSE
AT := 
else
AT := @
endif

##### Recipes.
define recipe_makedep
	$(AT)echo "Generating $@"
	$(AT)mkdir -p $(dir $@)
	$(AT)$(CC) $1 -MM -MG $< > $(@:.o=.d)
	$(AT)mv -f $(@:.o=.d) $(@:.o=.d).tmp
	$(AT)sed -e 's|.*:|$@:|' < $(@:.o=.d).tmp > $(@:.o=.d)
	$(AT)sed -e 's/.*://' -e 's/\\$$//' < $(@:.o=.d).tmp | fmt -1 | \
        sed -e 's/^ *//' -e 's/$$/:/' >> $(@:.o=.d)
	$(AT)rm -f $(@:.o=.d).tmp
endef

define recipe_compile
	$(AT)echo "Compiling $<"
	$(AT)$(CC) $1 -c -o $@ $<
	$(call recipe_makdep, $1)
endef

define recipe_archive
	$(AT)echo "Archiving $@"
	$(AT)$(AR) cr $@ $^
endef

define recipe_translate
	$(AT)echo "Translating $<"
	$(AT)mkdir -p $(dir $@)
	$(AT)$(SC) $(SFLAGS) -e -t $< -output-file $@
endef

# $1: object files
# $2: link flags
define recipe_link
    @echo "Linking $@"
	$(AT)$(CC) -o $@ $(1) $(2)
endef


define recipe_mkdir
	@mkdir -p $@
endef
