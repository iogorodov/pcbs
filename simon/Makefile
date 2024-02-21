PROGRAMMER	:=	usbtiny
TARGET		:=	attiny13

CC 			:=	avr-gcc
CFLAGS		:=	-Os -flto -Wall
LDFLAGS		:=	-flto
OBJCOPY		:=	avr-objcopy
KICAD		:=	/opt/homebrew/Caskroom/kicad/7.0.9/KiCad/KiCad.app/Contents/MacOS/kicad-cli
AVRDUDE		:=  avrdude

SRCDIR		:=	src
OUTDIR		:=	.out
DOCSDIR		:=	docs

################################################################################
SOURCES		:= $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS		:= $(patsubst $(SRCDIR)/%,$(OUTDIR)/%,$(SOURCES:.c=.o))
DOCS		:= $(DOCSDIR)/sch.svg $(DOCSDIR)/pcb-back.pdf $(DOCSDIR)/pcb.svg

all: $(OUTDIR)/_program.hex

install: $(OUTDIR)/_program.hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(TARGET) -U flash:w:$<

fuse: $(OUTDIR)/_fuse.bin
# 	@echo `hexdump -e '"0x%02X\n"' -s 0 -n 1 $<`" "`hexdump -e '"0x%02X\n"' -s 1 -n 1 $<`" "`hexdump -e '"0x%02X\n"' -s 2 -n 1 $<`
	$(AVRDUDE) -c $(PROGRAMMER) -p $(TARGET) -U lfuse:w:`hexdump -e '"0x%02X\n"' -s 0 -n 1 $<`:m -U hfuse:w:`hexdump -e '"0x%02X\n"' -s 1 -n 1 $<`:m -U efuse:w:`hexdump -e '"0x%02X\n"' -s 2 -n 1 $<`:m

clean:
	@rm -rf $(OUTDIR)

$(OUTDIR)/_program.hex: $(OBJECTS)
	$(CC) -mmcu=$(TARGET) $(LDFLAGS) -o $(OUTDIR)/_program.elf $^
	$(OBJCOPY) -O ihex -R .fuse $(OUTDIR)/_program.elf $@
	avr-size --mcu=$(TARGET) -d $(OUTDIR)/_program.elf
	@rm $(OUTDIR)/_program.elf

$(OUTDIR)/_fuse.bin: $(OBJECTS)
	$(CC) -mmcu=$(TARGET) $(LDFLAGS) -o $(OUTDIR)/_program.elf $^
	$(OBJCOPY) -j .fuse -O binary -S $(OUTDIR)/_program.elf $@
	@rm $(OUTDIR)/_program.elf

-include $(OBJECTS:.o=.d)

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -mmcu=$(TARGET) $(CFLAGS) $< -o $@
	$(CC) -MM -mmcu=$(TARGET) $(CFLAGS) -MT $(OUTDIR)/$*.o $< > $(OUTDIR)/$*.d

docs: $(DOCS)

$(DOCSDIR)/sch.svg: ./sch/simon.kicad_sch
	$(KICAD) sch export svg -enO -o $(OUTDIR)/sch $<
	mv $(OUTDIR)/sch/* $@
	rm -rf $(OUTDIR)/sch

$(DOCSDIR)/pcb-back.pdf: ./sch/simon.kicad_pcb
	$(KICAD) pcb export pdf -l B.Cu,Edge.Cuts --black-and-white --drill-shape-opt 1 -o $@ $<

$(DOCSDIR)/pcb.svg: ./sch/simon.kicad_pcb
	$(KICAD) pcb export svg -l B.Cu,F.Cu,Edge.Cuts --exclude-drawing-sheet --page-size-mode 2 -o $@ $<

clean_docs:
	@rm -rf $(DOCS)

.PHONY: all install fuse clean docs clean_docs
