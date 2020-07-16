CC = g++
SHELL = bash
CFLAGS = -g -Wall

HALIDE_BIN_PATH ?= ../../../Halide-to-Hardware
HALIDE_SRC_PATH ?= $(HALIDE_BIN_PATH)
HALIDE_HW_PATH ?= $(HALIDE_BIN_PATH)/apps/hardware_benchmarks
HALIDE_DISTRIB_PATH ?= $(HALIDE_BIN_PATH)/distrib
HWSUPPORT ?= $(HALIDE_HW_PATH)/hw_support
CLOCKWORK_PATH ?= ../../clockwork
ISL_PATH ?= $(CLOCKWORK_PATH)/barvinok-0.41/isl
CLOCKWORK_CXX_FLAGS = -std=c++17 -I$(CLOCKWORK_PATH) -I$(CLOCKWORK_PATH)/include -I$(ISL_PATH) -fPIC
CLOCKWORK_LD_FLAGS = -L$(CLOCKWORK_PATH)/lib -L$(ISL_PATH) -Wl,-rpath,$(CLOCKWORK_PATH)/lib
CLOCKWORK_LD_FLAGS += -lclkwrk -lbarvinok -lisl -lntl -lgmp -lpolylibgmp -lpthread

APP_BIN ?= $(HALIDE_HW_PATH)/apps/unsharp/bin
BIN ?= ./bin
OUTPUT ?= ./output
LDFLAGS ?=
COREIR_DIR ?= ../../coreir
FUNCBUF_DIR ?= ../../BufferMapping/cfunc

# For CoreIR generation
COREIR_CXX_FLAGS = -I $(COREIR_DIR)/include -L$(COREIR_DIR)/lib -Wl,-rpath,$(COREIR_DIR)/lib
COREIR_CXX_FLAGS += -I $(FUNCBUF_DIR)/include -L$(FUNCBUF_DIR)/bin -Wl,-rpath,$(FUNCBUF_DIR)/bin
CXXFLAGS += $(COREIR_CXX_FLAGS)

# For running testbench
CXX_FLAGS += -I $(APP_BIN) -L $(APP_BIN)
RDAI_PLATFORM_CXXFLAGS 		= -I ./include
RDAI_PLATFORM_CXXFLAGS 		+= -I$(BIN)

default: all

all: $(BIN)/clockwork_testscript.o $(BIN)/unoptimized_unsharp.o $(HWSUPPORT)/$(BIN)/hardware_process_helper.o
	@-mkdir -p $(BIN)
	@-mkdir -p $(OUTPUT)
	$(CC) $(CXXFLAGS) $(RDAI_PLATFORM_CXXFLAGS) -o $(BIN)/test_clockwork_ops test_clockwork_ops.cpp $(BIN)/clockwork_testscript.o $(BIN)/unoptimized_unsharp.o -lpthread -lpng16 -ljpeg

$(BIN)/rdai_clockwork_platform.o: $(RDAI_DIR)/platform_runtimes/clockwork_sim/src/rdai_clockwork_platform.cpp
	$(CXX) $(CXXFLAGS) -I$(CLOCKWORK_PATH) $(RDAI_PLATFORM_CXXFLAGS) -c $^ -o $@
$(BIN)/clockwork_testscript.o: $(APP_BIN)/clockwork_testscript.cpp $(APP_BIN)/unoptimized_unsharp.cpp $(APP_BIN)/clockwork_testscript.h
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -I$(CLOCKWORK_PATH)  -c $< -o $@
$(BIN)/unoptimized_unsharp.o: $(APP_BIN)/unoptimized_unsharp.cpp $(APP_BIN)/unoptimized_unsharp.h $(HWSUPPORT)/$(BIN)/hardware_process_helper.o
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -I$(CLOCKWORK_PATH)  -c $< -o $@

run: $(BIN)/test_clockwork_ops
	@-mkdir -p $(BIN)
	$(BIN)/test_clockwork_ops input/input.png

clean:
	rm -rf $(BIN)
	rm -rf $(OUTPUT)
	rm -rf ./unoptimized_unsharp_debug.csv