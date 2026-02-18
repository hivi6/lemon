BUILD_DIR := build
FINAL_PATH := $(BUILD_DIR)/lemon

SRC_PATH := src
INC_PATH := include

C_FILES := $(shell find $(SRC_PATH) -name '*.c')
H_FILES := $(shell find $(INC_PATH) -name '*.h')

$(FINAL_PATH): $(BUILD_DIR) $(C_FILES) $(H_FILES)
	$(CC) -o $(FINAL_PATH) -I$(INC_PATH) $(C_FILES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

