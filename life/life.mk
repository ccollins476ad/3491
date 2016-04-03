##### Compiler input
LIFE_IDIRS :=
LIFE_IDIRS += $(BASE_DIR)
LIFE_CFLAGS = $(call build_cflags, $(LIFE_IDIRS))

LIFE_LEAF_SRCS :=
LIFE_LEAF_SRCS += data_gesture.c
LIFE_LEAF_SRCS += data_image.c
LIFE_LEAF_SRCS += data_shp1.c
LIFE_LEAF_SRCS += data_terr.c
LIFE_LEAF_SRCS += gesture.c
LIFE_LEAF_SRCS += fsm.c
LIFE_LEAF_SRCS += image.c
LIFE_LEAF_SRCS += input.c
LIFE_LEAF_SRCS += input_state.c
LIFE_LEAF_SRCS += level.c
LIFE_LEAF_SRCS += phys.c
LIFE_LEAF_SRCS += scroll.c
LIFE_LEAF_SRCS += stats.c
LIFE_LEAF_SRCS += terr.c
LIFE_LEAF_SRCS += timing.c
LIFE_LEAF_SRCS += world.c
LIFE_LEAF_SRCS += world_view.c
LIFE_LEAF_SRCS += zone.c

LIFE_LEAF_OBJS := $(LIFE_LEAF_SRCS:.c=.o)
LIFE_SRCS := $(addprefix $(LIB_LIFE_DIR)/, $(LIFE_LEAF_SRCS))
LIFE_OBJS := $(addprefix $(LIB_LIFE_BIN_DIR)/, $(LIFE_LEAF_OBJS))
LIFE_DEPS := $(LIFE_OBJS:.o=.d)

##### Rules.
.PHONY: life
life : $(LIB_LIFE_BIN_DIR) $(LIB_LIFE_BIN)

$(LIB_LIFE_BIN_DIR):
	$(call recipe_mkdir)

$(LIB_LIFE_BIN): $(LIFE_OBJS)
	$(recipe_archive)

$(LIB_LIFE_BIN_DIR)/%.o: $(LIB_LIFE_DIR)/%.c
	$(call recipe_compile, $(LIFE_CFLAGS))

.PHONY: life_clean
life_clean:
	$(RM) $(LIFE_OBJS) $(LIB_LIFE_BIN) $(LIFE_DEPS)

ifneq ($(MAKECMDGOALS),life_clean)
    -include $(LIFE_DEPS)
endif

