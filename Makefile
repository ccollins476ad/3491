BASE_DIR := .

include $(BASE_DIR)/mk/paths.mk
include $(MK_DIR)/compile.mk

include $(LIB_GEN_DIR)/gen.mk

include $(LIB_GFX_DIR)/gfx.mk

#include $(LIB_LIFE_DIR)/life.mk

#include $(DEMO_DEMO01_DIR)/demo01.mk

include $(TOOL_IMGED_DIR)/imged.mk

##### Rules.
.PHONY: clean
clean: gen_clean gfx_clean life_clean demo01_clean

##### Dependencies.
DEPS := $(OBJS:.o=.d)
ifneq ($(MAKECMDGOALS),clean)
    -include $(DEPS)
endif

