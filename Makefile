
export BUILD_DIR ?= $(CURDIR)/build

all: copy-file gen-file check-file copy-check buf-scan test-buf driver
.PHONY: copy-file gen-file check-file copy-check buf-scan test-buf driver

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

test-buf:
	make -C test_buf_app

driver:
	make -C driver
