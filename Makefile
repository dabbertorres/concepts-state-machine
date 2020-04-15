CPP      := /usr/local/opt/llvm@10/bin/clang++
override CPPFLAGS += -std=c++2a -Wall -Wextra -fdiagnostics-color=always -Iinclude -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk -stdlib=libc++ -I/usr/local/opt/llvm@10/include
override LDFLAGS  += -L/usr/local/opt/llvm@10/lib -Wl,-rpath,/usr/local/opt/llvm@10/lib

DEP_DIR   ?= .dep
BUILD_DIR ?= build

BIN     ?= state
SRC     := $(shell find src -type f -name '*.cpp')
OBJ     := $(SRC:src/%.cpp=$(BUILD_DIR)/%.o)
DEP     := $(SRC:src/%.cpp=$(DEP_DIR)/%.d)
COMP_DB := $(DEP:%.d=%.json)

DEPFLAGS = -MP -MT $(BUILD_DIR)/$*.o -MF $(DEP_DIR)/$*.d

.PHONY: clean clean-full comp-db

$(BIN): $(OBJ)
	@-mkdir -p $(@D)
	$(CPP) -o $@ $^ $(LDFLAGS)

$(OBJ): $(BUILD_DIR)/%.o: src/%.cpp $(DEP_DIR)
	@-mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -MMD $(DEPFLAGS) -o $@ -c $<

$(DEP_DIR):
	@-mkdir -p $@

-include $(DEP)

comp-db: compile_commands.json
compile_commands.json: $(COMP_DB)
	sed -e '1s/^/[/' -e '$$s/,$$/]/' $^ > $@

$(COMP_DB): $(DEP_DIR)/%.json: src/%.cpp
	@-mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -MM $(DEPFLAGS) -MJ $@ $<

clean:
	@-rm -f $(BIN)
	@-rm -f $(OBJ)
	@-rm -rf $(BUILD_DIR)

clean-full: clean
	@-rm -rf $(DEP_DIR)
	@-rm -f compile_commands.json
