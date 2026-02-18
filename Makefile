BUILD_DIR := build
FINAL_PATH := $(BUILD_DIR)/lemon

C_FILES := $(shell find src -name '*.c')

$(FINAL_PATH): $(BUILD_DIR) $(C_FILES)
	$(CC) -o $(FINAL_PATH) $(C_FILES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

