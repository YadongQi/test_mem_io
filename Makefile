
export BUILD_DIR ?= $(CURDIR)/build

all: copy-file gen-file

copy-file:
	make -C copy

gen-file:
	make -C generate