##### Compiler input
GEN_IDIRS :=
GEN_IDIRS += $(BASE_DIR)
GEN_CFLAGS := $(call build_cflags, $(GEN_IDIRS))

GEN_LEAF_SRCS :=
GEN_LEAF_SRCS += container.c
GEN_LEAF_SRCS += direction.c
GEN_LEAF_SRCS += gen_dbg.c
GEN_LEAF_SRCS += gen_num.c
GEN_LEAF_SRCS += gen_shape.c
GEN_LEAF_SRCS += gen_text.c
GEN_LEAF_SRCS += glog.c
GEN_LEAF_SRCS += phys.c
GEN_LEAF_SRCS += res.c
GEN_LEAF_SRCS += strlcat.c
GEN_LEAF_SRCS += ticker.c

GEN_LEAF_OBJS := $(GEN_LEAF_SRCS:.c=.o)
GEN_SRCS := $(addprefix $(LIB_GEN_DIR)/, $(GEN_LEAF_SRCS))
GEN_OBJS := $(addprefix $(LIB_GEN_BIN_DIR)/, $(GEN_LEAF_OBJS))
GEN_DEPS := $(GEN_OBJS:.o=.d)

##### Rules.
.PHONY: gen
.PHONY: test_gen
.PHONY: test_gen_clean
test_gen:
test_gen_clean:

gen : $(LIB_GEN_BIN_DIR) $(LIB_GEN_BIN) test_gen

$(LIB_GEN_BIN_DIR):
	$(call recipe_mkdir)

$(LIB_GEN_BIN): $(GEN_OBJS)
	$(recipe_archive)

$(LIB_GEN_BIN_DIR)/%.o: $(LIB_GEN_DIR)/%.c
	$(call recipe_compile, $(GEN_CFLAGS))

.PHONY: gen_clean
gen_clean: test_gen_clean
	$(RM) $(GEN_OBJS) $(LIB_GEN_BIN) $(GEN_DEPS)

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
    -include $(GEN_DEPS)
endif
