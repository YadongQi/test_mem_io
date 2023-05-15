
export BUILD_DIR ?= $(CURDIR)/build

all: copy-file gen-file check-file copy-check buf-scan

copy-file:
	make -C copy

gen-file:
	make -C generate

check-file:
	make -C checkfile

copy-check:
	make -C copy_check

buf-scan:
	make -C buf_scan
