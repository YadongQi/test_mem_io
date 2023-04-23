
export BUILD_DIR ?= $(CURDIR)/build

all: copy-file gen-file check-file

copy-file:
	make -C copy

gen-file:
	make -C generate

check-file:
	make -C checkfile