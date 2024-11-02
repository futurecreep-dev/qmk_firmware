OPT_DEFS += -DFACTORY_TEST_ENABLE

KEYCHRON_COMMON_DIR = common
SRC += \
	$(KEYCHRON_COMMON_DIR)/keychron_task.c \
    $(KEYCHRON_COMMON_DIR)/keychron_common.c \
    $(KEYCHRON_COMMON_DIR)/factory_test.c \
    $(KEYCHRON_COMMON_DIR)/hybrid_switch_animation.c

VPATH += $(TOP_DIR)/keyboards/keychron/$(KEYCHRON_COMMON_DIR)

