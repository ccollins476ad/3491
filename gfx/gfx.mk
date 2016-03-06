##### Compiler input
GFX_IDIRS :=
GFX_IDIRS += $(BASE_DIR)
GFX_CFLAGS = $(call build_cflags, $(GFX_IDIRS))

GFX_LEAF_SRCS :=
GFX_LEAF_SRCS += animation.c
GFX_LEAF_SRCS += canvas.c
GFX_LEAF_SRCS += fader.c
GFX_LEAF_SRCS += gfx.c
GFX_LEAF_SRCS += image.c
GFX_LEAF_SRCS += periph.c
GFX_LEAF_SRCS += resize.c
GFX_LEAF_SRCS += screen.c
GFX_LEAF_SRCS += smart_resize.c
GFX_LEAF_SRCS += tile.c
GFX_LEAF_SRCS += tile_shape.c

GFX_LEAF_OBJS := $(GFX_LEAF_SRCS:.c=.o)
GFX_SRCS := $(addprefix $(LIB_GFX_DIR)/, $(GFX_LEAF_SRCS))
GFX_OBJS := $(addprefix $(LIB_GFX_BIN_DIR)/, $(GFX_LEAF_OBJS))
GFX_DEPS := $(GFX_OBJS:.o=.d)

##### Rules.
.PHONY: gfx
gfx : $(LIB_GFX_BIN)

$(LIB_GFX_BIN): $(GFX_OBJS)
	$(recipe_archive)

$(LIB_GFX_BIN_DIR)/%.o: $(LIB_GFX_DIR)/%.c $(LIB_GFX_BIN_DIR)/%.d
	$(call recipe_compile, $(GFX_CFLAGS))

$(LIB_GFX_BIN_DIR)/%.d: $(LIB_GFX_DIR)/%.c
	$(call recipe_makedep, $(GFX_CFLAGS))

.PHONY: gfx_clean
gfx_clean:
	$(RM) -f $(GFX_OBJS) $(LIB_GFX_BIN) $(GFX_DEPS)

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
    -include $(GFX_DEPS)
endif
