CC = g++
SHELL = bash
CFLAGS = -g -Wall

HALIDE_BIN_PATH ?= ../../Halide-to-Hardware
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
IMAGES ?= $(HALIDE_HW_PATH)/images
LDFLAGS ?=
COREIR_DIR ?= ../../coreir
FUNCBUF_DIR ?= ../../BufferMapping/cfunc

# For CoreIR generation
COREIR_CXX_FLAGS = -I $(COREIR_DIR)/include -L$(COREIR_DIR)/lib -Wl,-rpath,$(COREIR_DIR)/lib
COREIR_CXX_FLAGS += -I $(FUNCBUF_DIR)/include -L$(FUNCBUF_DIR)/bin -Wl,-rpath,$(FUNCBUF_DIR)/bin
CXXFLAGS += $(COREIR_CXX_FLAGS)

# For running testbench
CXX_FLAGS += -I $(APP_BIN) -L $(APP_BIN)


default: all

all: $(BIN)/clockwork_testscript.o $(BIN)/unoptimized_unsharp.o $(HWSUPPORT)/$(BIN)/hardware_process_helper.o
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -o $(BIN)/aha_clockwork_tb aha_clockwork_tb.cpp $(BIN)/clockwork_testscript.o $(BIN)/unoptimized_unsharp.o

$(BIN)/clockwork_testscript.o: $(APP_BIN)/clockwork_testscript.cpp $(APP_BIN)/unoptimized_unsharp.cpp $(APP_BIN)/clockwork_testscript.h
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -I$(CLOCKWORK_PATH)  -c $< -o $@

$(BIN)/unoptimized_unsharp.o: $(APP_BIN)/unoptimized_unsharp.cpp $(APP_BIN)/unoptimized_unsharp.h $(HWSUPPORT)/$(BIN)/hardware_process_helper.o
	@-mkdir -p $(BIN)
	$(CC) $(CXXFLAGS) -I$(CLOCKWORK_PATH)  -c $< -o $@

clean:
	rm -rf $(BIN)
