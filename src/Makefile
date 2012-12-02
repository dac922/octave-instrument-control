#OCT = instrument-control.oct
SUBDIRS = usbtmc tcp gpib

MKOCTFILE ?= mkoctfile

all:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir; done
	
clean:
	 @for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
	 rm -f *.oct
