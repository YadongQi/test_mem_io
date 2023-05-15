SRCS := test_buf_app.c


BUILD_DIR ?= $(CURDIR)/build
OBJDIR ?= $(BUILD_DIR)/obj

OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -g -static $< -o $@

PROG_NAME := test-buf
TARGET := $(BUILD_DIR)/$(PROG_NAME)

#LDFLAGS := -static
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(TARGET)
	$(RM) -r $(OBJDIR)
	$(RM) -r $(BUILD_DIR)

all: $(PROG_NAME)
