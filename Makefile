# Minimal Makefile for Phase 4
CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -O2 -fPIC -Isrc
PYTHON = python3
PYTHON_INCLUDES = $(shell python3-config --includes)
PYTHON_LIBS = $(shell python3-config --ldflags)

SRC_DIR = src
BIN_DIR = bin

RUNTIME_SOURCES = \
src/runtime/main.c \
src/runtime/context.c \
src/runtime/logger.c \
src/runtime/config_loader.c \
src/runtime/py_embed.c \
src/runtime/scheduler.c \
src/function_blocks/fb_common.c \
src/function_blocks/fb_pid.c \
src/function_blocks/fb_first_order.c \
src/function_blocks/fb_ramp.c \
src/function_blocks/fb_limit.c

RUNTIME_TARGET = $(BIN_DIR)/plcopen_runtime

.PHONY: all build runtime clean lint test

all: build runtime

build:
	@echo "Building Python C extension..."
	$(PYTHON) setup.py build_ext --inplace

runtime:
	@echo "Building runtime executable..."
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(PYTHON_INCLUDES) -o $(RUNTIME_TARGET) $(RUNTIME_SOURCES) -lpthread -lm -lpython3.13 $(PYTHON_LIBS)

lint:
	@echo "Running Python code quality checks..."
	@echo "==> flake8"
	-$(PYTHON) -m flake8 python/ setup.py
	@echo "==> black (check only)"
	-$(PYTHON) -m black --check --line-length=88 python/ setup.py
	@echo "==> pylint"
	-$(PYTHON) -m pylint --max-line-length=88 python/plcopen/ python/examples/*.py setup.py
	@echo ""
	@echo "Running C code quality checks..."
	@echo "==> cppcheck (if available)"
	-which cppcheck > /dev/null && cppcheck --enable=all --std=c11 \
		--suppress=missingIncludeSystem --suppress=unusedFunction src/ || \
		echo "cppcheck not found, skipping C static analysis"

format:
	@echo "Auto-formatting Python code with black..."
	$(PYTHON) -m black --line-length=88 python/ setup.py

test:
	@echo "Running tests..."
	$(PYTHON) -m pytest python/tests/ -v

clean:
	rm -rf $(BIN_DIR) build/ *.so __pycache__
