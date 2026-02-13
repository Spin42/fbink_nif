# Makefile for fbink_nif
# Cross-compilation aware for Nerves
#
# Two modes:
#   1. Cross-compile (Nerves): CROSSCOMPILE is set. FBInk headers and libs
#      are in the Buildroot sysroot - the cross-compiler finds them via --sysroot.
#      No FBINK_DIR needed.
#   2. Host dev: FBINK_DIR points at a local FBInk source tree for headers + library.

PREFIX = $(MIX_APP_PATH)/priv
BUILD  = $(MIX_APP_PATH)/obj

# Erlang NIF includes - Nerves sets ERTS_INCLUDE_DIR; fall back to erl query for host
ifdef ERTS_INCLUDE_DIR
	ERL_INCLUDE_DIR = $(ERTS_INCLUDE_DIR)
else
	ERL_INCLUDE_DIR ?= $(shell erl -noshell -eval "io:format(\"~s/erts-~s/include\", [code:root_dir(), erlang:system_info(version)])." -s init stop 2>/dev/null)
endif

# Compiler settings
CC     ?= cc
CFLAGS ?= -O2
CFLAGS += -Wall -Wextra -Wno-unused-parameter
CFLAGS += -std=c11
CFLAGS += -fPIC
CFLAGS += -I$(ERL_INCLUDE_DIR)

LDFLAGS += -lfbink -lm

# Platform detection for shared library extension
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	SO_EXT = dylib
	SO_LDFLAGS = -dynamiclib -undefined dynamic_lookup
else
	SO_EXT = so
	SO_LDFLAGS = -shared
endif

# --- Mode-specific flags ---

ifneq ($(CROSSCOMPILE),)
	# Cross-compile (Nerves): fbink.h and libfbink.so are in the sysroot.
	# The cross-compiler's --sysroot handles include/lib paths automatically.
	# Link dynamically against the target's libfbink.so.
else
	# Host dev: use local FBInk source tree
	FBINK_DIR ?= $(shell pwd)/../FBInk
	CFLAGS  += -I$(FBINK_DIR)
	LDFLAGS := -L$(FBINK_DIR) -L$(FBINK_DIR)/Release $(LDFLAGS)
endif

# Target
NIF = $(PREFIX)/fbink_nif.$(SO_EXT)

# Sources
SOURCES = c_src/fbink_nif.c
OBJECTS = $(SOURCES:c_src/%.c=$(BUILD)/%.o)

# Default target
all: $(PREFIX) $(BUILD) $(NIF)

$(PREFIX):
	@mkdir -p $(PREFIX)

$(BUILD):
	@mkdir -p $(BUILD)

$(NIF): $(OBJECTS)
	$(CC) $(SO_LDFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: c_src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD) $(NIF)

.PHONY: all clean
