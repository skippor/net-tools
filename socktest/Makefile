TARGET  = pcapdump

INCLUDE ?= ./include
LIBDIR ?= ./libs
SRCS    = $(wildcard *.c)
OBJS    = $(SRCS:.c=.o)
CFLAGS   = -g -pipe $(INCLUDE:%=-I%) -DAPPNAME=\"$(TARGET)\"

TARGETDIR ?= ./

ifneq ($(DEBUG),)
	CFLAGS += -DDEBUG
endif

.PHONY: clean

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

install:
	mkdir -p $(TARGETDIR)
	cp -rf $(TARGET) $(TARGETDIR)

clean:
	rm -f $(OBJS) $(TARGET)
