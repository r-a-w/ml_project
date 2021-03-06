CC := g++
LD := g++
DEBUG := -g
PWD := $(shell pwd)
CFLAGS := -I$(PWD)/include
DEPS := placeholder.h

SRC_BASE := $(PWD)/src/
SRC_MODULES := data_containers utilities trainers test
INCLUDE_MODULES := #empty
INCLUDE_BASE := $(PWD)/include/

SRC_DIR := $(addprefix $(SRC_BASE), $(SRC_MODULES))
BUILD_DIR := $(addprefix $(PWD)/build/, $(SRC_MODULES))

SRC:= $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.cpp))
INC := -I$(INCLUDE_BASE) $(addprefix -I$(INCLUDE_BASE), $(INCLUDE_MODULES))
OBJ := $(subst src,build,$(SRC))
OBJ := $(patsubst %.cpp,%.o,$(OBJ))
TARGET := test #build/test/test

#$(info $(OBJ))

#$(TARGET): $(OBJ)
#	$(CC) $^ -o $@ 

vpath %.cpp $(SRC_DIR)

define make_goal
$1/%.o: %.cpp
	$(CC) $(INC) $(DEBUG) -c $$< -o $$@ 
endef

.PHONY: all checkdirs clean

all: checkdirs $(TARGET) 

$(TARGET): $(OBJ)
	$(LD) $(INC) $^ -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir, $(BUILD_DIR), $(eval $(call make_goal,$(bdir))))





