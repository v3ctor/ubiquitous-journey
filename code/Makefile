# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)

TARGET_EXEC := main
BUILD_DIR := ./build
SRC_DIRS := ./src
CXX=clang++
override CPPFLAGS += -I./include -std=c++17 -Wall -Wextra -march=native -MMD -MP

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: release
release:
	make "$(TARGET_EXEC)" CPPFLAGS="-DNDEBUG -fno-rtti -Ofast"

.PHONY: debug
debug:
	make "$(TARGET_EXEC)" CPPFLAGS="-ggdb -Og -fsanitize=address" LDFLAGS="-lasan"

$(TARGET_EXEC): $(OBJS)
	$(CXX) -march=native -mavx512f $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -march=native $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm $(TARGET_EXEC)
	rm -r $(BUILD_DIR)

-include $(DEPS)
