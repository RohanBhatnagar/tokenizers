.PHONY: all build run clean

all: build

build:
	@mkdir -p build
	@cd build && cmake .. && make

run: build
	@cd build && ./tokenizer

clean:
	@rm -rf build
	@rm -f vocab.txt

help:
	@echo "Available targets:"
	@echo "  make build  - Build the project"
	@echo "  make run    - Build and run the tokenizer"
	@echo "  make clean  - Clean build artifacts"
