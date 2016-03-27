##### Compiler input
DEMO01_IDIRS := $(BASE_DIR)

DEMO01_CFLAGS = $(call build_cflags, $(DEMO01_IDIRS))

DEMO01_LDIRS += $(LIB_GEN_BIN_DIR)
DEMO01_LDIRS += $(LIB_GFX_BIN_DIR)
DEMO01_LDIRS += $(LIB_LIFE_BIN_DIR)

DEMO01_LIBS += life
DEMO01_LIBS += gfx
DEMO01_LIBS += gen
DEMO01_LIBS += alleg
DEMO01_LIBS += m

ifeq ($(TARGET_OS),win32)
DEMO01_LIBS += user32 gdi32 comdlg32 ole32 dinput ddraw dxguid winmm dsound
endif

DEMO01_LFLAGS = $(call build_lflags,$(DEMO01_LDIRS),$(DEMO01_LIBS))

DEMO01_LEAF_SRCS := main.c

DEMO01_LEAF_OBJS := $(DEMO01_LEAF_SRCS:.c=.o)
DEMO01_SRCS := $(addprefix $(DEMO_DEMO01_DIR)/, $(DEMO01_LEAF_SRCS))
DEMO01_OBJS := $(addprefix $(DEMO_DEMO01_BIN_DIR)/, $(DEMO01_LEAF_OBJS))
DEMO01_DEPS := $(DEMO01_OBJS:.o=.d)

DEP_LIBS := $(LIB_GEN_BIN) $(LIB_GFX_BIN) $(LIB_LIFE_BIN)

##### Rules.
demo01: $(DEMO_DEMO01_BIN_DIR) $(DEMO_DEMO01_BIN)

$(DEMO_DEMO01_BIN_DIR):
	$(call recipe_mkdir)

$(DEMO_DEMO01_BIN): $(DEMO01_OBJS) $(DEP_LIBS)
	$(call recipe_link,$(DEMO01_OBJS),$(DEMO01_LFLAGS))

$(DEMO_DEMO01_BIN_DIR)/%.o: $(DEMO_DEMO01_DIR)/%.c
	$(call recipe_compile,$(DEMO01_CFLAGS))

.PHONY: demo01_clean
demo01_clean: gen_clean gfx_clean life_clean
	$(RM) -f $(DEMO01_OBJS) $(DEMO_DEMO01_BIN) $(DEMO01_DEPS)

ifneq ($(MAKECMDGOALS),demo01_clean)
    -include $(DEMO01_DEPS)
endif

