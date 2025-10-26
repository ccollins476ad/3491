##### Compiler input
IMGED_IDIRS := $(BASE_DIR) /home/ccollins/tmp/allegro-4.4.3.1/include

IMGED_CFLAGS = $(call build_cflags, $(IMGED_IDIRS))
IMGED_CFLAGS += -Wno-poison-system-directories

IMGED_LDIRS += $(LIB_GFX_BIN_DIR)
IMGED_LDIRS += $(LIB_GEN_BIN_DIR)

IMGED_LIBS :=
IMGED_LIBS += gfx
IMGED_LIBS += gen

ifeq ($(TARGET_OS),win32)
IMGED_LIBS += allegro-4.4.2-static-mt
IMGED_LIBS += kernel32 user32 gdi32 comdlg32 ole32 dinput ddraw dxguid winmm dsound
else
IMGED_LIBS += alleg
endif

IMGED_LFLAGS = $(call build_lflags,$(IMGED_LDIRS),$(IMGED_LIBS)) -lm

IMGED_LEAF_SRCS := imged.c

IMGED_LEAF_OBJS := $(IMGED_LEAF_SRCS:.c=.o)
IMGED_SRCS := $(addprefix $(TOOL_IMGED_DIR)/, $(IMGED_LEAF_SRCS))
IMGED_OBJS := $(addprefix $(TOOL_IMGED_BIN_DIR)/, $(IMGED_LEAF_OBJS))
IMGED_DEPS := $(IMGED_OBJS:.o=.d)

DEP_LIBS :=
DEP_LIBS += $(LIB_GFX_BIN)
DEP_LIBS += $(LIB_GEN_BIN)

##### Rules.
imged: $(TOOL_IMGED_BIN)

$(TOOL_IMGED_BIN): $(IMGED_OBJS) $(DEP_LIBS)
	$(call recipe_link,$(IMGED_OBJS),$(IMGED_LFLAGS))

$(TOOL_IMGED_BIN_DIR)/%.o: $(TOOL_IMGED_DIR)/%.c
	$(call recipe_compile,$(IMGED_CFLAGS))

.PHONY: imged_clean
imged_clean:
	$(RM) -f $(IMGED_OBJS) $(TOOL_IMGED_BIN) $(IMGED_DEPS)

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
    -include $(IMGED_DEPS)
endif

