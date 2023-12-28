PROGRAMMER	:=	usbtiny
TARGET		:=	attiny45

CC 			:=	avr-gcc
CFLAGS		:=	-mmcu=$(TARGET) -Os -flto -Wall
LDFLAGS		:=	-mmcu=$(TARGET) -flto
OBJCOPY		:=	avr-objcopy

NAME		:= 	program
SRCDIR		:=	src
OUTDIR		:=	.out

################################################################################
SOURCES     := $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS		:= $(patsubst $(SRCDIR)/%,$(OUTDIR)/%,$(SOURCES:.c=.o))

all: $(OUTDIR)/$(NAME).hex

install: $(OUTDIR)/$(NAME).hex
	avrdude -c $(PROGRAMMER) -p $(TARGET) -U flash:w:$<

clean:
	rm -rf $(OUTDIR)

$(OUTDIR)/$(NAME).hex: $(OUTDIR)/$(NAME).elf
	$(OBJCOPY) -O ihex $< $@

$(OUTDIR)/$(NAME).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all clean
