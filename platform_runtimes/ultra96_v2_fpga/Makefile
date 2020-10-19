CC = g++
SHELL = bash
CFLAGS = -g -Wall

HALIDE_BIN_PATH ?= ../../../Halide-to-Hardware
HALIDE_SRC_PATH ?= $(HALIDE_BIN_PATH)
HALIDE_HW_PATH ?= $(HALIDE_BIN_PATH)/apps/hardware_benchmarks
HALIDE_DISTRIB_PATH ?= $(HALIDE_BIN_PATH)/distrib
HWSUPPORT ?= $(HALIDE_HW_PATH)/hw_support
CLOCKWORK_PATH ?= ../../clockwork

APP_BIN ?= $(HALIDE_HW_PATH)/tests/conv_3_3/bin
BIN ?= ./bin
OUTPUT ?= ./output
LDFLAGS ?=

# For CoreIR generation
COREIR_CXX_FLAGS = -I $(COREIR_DIR)/include -L$(COREIR_DIR)/lib -Wl,-rpath,$(COREIR_DIR)/lib
COREIR_CXX_FLAGS += -I $(FUNCBUF_DIR)/include -L$(FUNCBUF_DIR)/bin -Wl,-rpath,$(FUNCBUF_DIR)/bin
CXXFLAGS += $(COREIR_CXX_FLAGS)

# For RDAI API
RDAI_DIR ?= $(HALIDE_BIN_PATH)/../rdai
RDAI_CXX_FLAGS = -I$(RDAI_DIR)/rdai_api
CXXFLAGS += $(RDAI_CXX_FLAGS)
# For running testbench
CXX_FLAGS 					+= -I $(APP_BIN)
RDAI_PLATFORM_CXXFLAGS 		= -I ./include -I$(BIN)

default: all

all: $(BIN)/clockwork_testscript.o $(BIN)/unoptimized_conv_3_3.o $(BIN)/rdai_clockwork_platform.o $(HWSUPPORT)/$(BIN)/hardware_process_helper.o
	@-mkdir -p $(BIN)
	@-mkdir -p $(OUTPUT)
	$(CC) $(CXXFLAGS) $(RDAI_PLATFORM_CXXFLAGS) -o $(BIN)/test_clockwork_ops test_clockwork_ops.cpp $(BIN)/clockwork_testscript.o $(BIN)/unoptimized_conv_3_3.o $(BIN)/rdai_clockwork_platform.o -lpthread -lpng16 -ljpeg

$(BIN)/rdai_clockwork_platform.o: ./src/rdai_clockwork_platform.cpp ./include/clockwork_sim_platform.h
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -I$(CLOCKWORK_PATH) $(RDAI_PLATFORM_CXXFLAGS) -c $< -o $@
$(BIN)/clockwork_testscript.o: $(APP_BIN)/clockwork_testscript.cpp $(APP_BIN)/unoptimized_conv_3_3.cpp $(APP_BIN)/clockwork_testscript.h
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -I$(CLOCKWORK_PATH)  -c $< -o $@
$(BIN)/unoptimized_conv_3_3.o: $(APP_BIN)/unoptimized_conv_3_3.cpp $(APP_BIN)/unoptimized_conv_3_3.h $(HWSUPPORT)/$(BIN)/hardware_process_helper.o
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -I$(CLOCKWORK_PATH)  -c $< -o $@

run: $(BIN)/test_clockwork_ops
	@-mkdir -p $(BIN)
	$(BIN)/test_clockwork_ops input/input.png

clean:
	rm -rf $(BIN)
	rm -rf $(OUTPUT)
	rm -rf ./unoptimized_conv_3_3_debug.csv