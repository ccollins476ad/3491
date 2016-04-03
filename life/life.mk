##### Compiler input
LIFE_IDIRS :=
LIFE_IDIRS += $(BASE_DIR)
LIFE_CFLAGS = $(call build_cflags, $(LIFE_IDIRS))

LIFE_LEAF_SRCS :=
LIFE_LEAF_SRCS += action.c
LIFE_LEAF_SRCS += being.c
LIFE_LEAF_SRCS += being_action.c
LIFE_LEAF_SRCS += being_batch.c
LIFE_LEAF_SRCS += being_dbg.c
LIFE_LEAF_SRCS += being_mgmt.c
LIFE_LEAF_SRCS += blast.c
LIFE_LEAF_SRCS += blood.c
LIFE_LEAF_SRCS += bullet.c
LIFE_LEAF_SRCS += carnage.c
LIFE_LEAF_SRCS += collision.c
LIFE_LEAF_SRCS += combat.c
LIFE_LEAF_SRCS += data_being.c
LIFE_LEAF_SRCS += data_blood.c
LIFE_LEAF_SRCS += data_gesture.c
LIFE_LEAF_SRCS += data_gun.c
LIFE_LEAF_SRCS += data_image.c
LIFE_LEAF_SRCS += data_shp1.c
LIFE_LEAF_SRCS += data_spark.c
LIFE_LEAF_SRCS += data_terr.c
LIFE_LEAF_SRCS += data_violence.c
LIFE_LEAF_SRCS += debris.c
LIFE_LEAF_SRCS += entity.c
LIFE_LEAF_SRCS += event.c
LIFE_LEAF_SRCS += fsm.c
LIFE_LEAF_SRCS += generated/const_names.c
LIFE_LEAF_SRCS += gesture.c
LIFE_LEAF_SRCS += gun.c
LIFE_LEAF_SRCS += hit_box.c
LIFE_LEAF_SRCS += image.c
LIFE_LEAF_SRCS += input.c
LIFE_LEAF_SRCS += input_state.c
LIFE_LEAF_SRCS += level.c
LIFE_LEAF_SRCS += object.c
LIFE_LEAF_SRCS += phys.c
LIFE_LEAF_SRCS += player.c
LIFE_LEAF_SRCS += scroll.c
LIFE_LEAF_SRCS += spark.c
LIFE_LEAF_SRCS += stats.c
LIFE_LEAF_SRCS += terr.c
LIFE_LEAF_SRCS += timing.c
LIFE_LEAF_SRCS += violence.c
LIFE_LEAF_SRCS += vision.c
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

LIFE_GENERATED_DIR := $(LIB_LIFE_DIR)/generated
LIFE_CONST_NAMES := $(LIFE_GENERATED_DIR)/const_names
$(LIFE_CONST_NAMES).c $(LIFE_CONST_NAMES).h: \
        $(TOOL_CONST_NAMES_BIN) \
        $(LIB_GEN_DIR)/direction.h \
        $(LIB_LIFE_DIR)/action_defs.h \
        $(LIB_LIFE_DIR)/fsm.h \
        $(LIB_LIFE_DIR)/being_defs.h \
        $(LIB_LIFE_DIR)/data_shp1.h \
        $(LIB_LIFE_DIR)/gesture.h \
        $(LIB_LIFE_DIR)/terr_defs.h \
        $(NULL)
	@echo "Generating $@"
	@mkdir -p $(dir $@)
	@$(TOOL_CONST_NAMES_BIN) $(LIFE_CONST_NAMES)

CLEAN_TARGETS := $(LIFE_OBJS)
CLEAN_TARGETS += $(LIB_LIFE_BIN)
CLEAN_TARGETS += $(LIFE_DEPS)
CLEAN_TARGETS += $(LIFE_CONST_NAMES).c
CLEAN_TARGETS += $(LIFE_CONST_NAMES).h

.PHONY: life_clean
life_clean:
	$(RM) -rf $(CLEAN_TARGETS)

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
    -include $(LIFE_DEPS)
endif
