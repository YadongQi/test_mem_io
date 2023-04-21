
export BUILD_DIR ?= $(CURDIR)/build

all: copy-file

copy-file:
	make -C copy