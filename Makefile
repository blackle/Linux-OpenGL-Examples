
DEBUG=false

CFLAGS := -fno-plt -O1 -std=gnu11 -nostartfiles -Wall -Wextra
CFLAGS += -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer
CFLAGS += -no-pie -fno-pic -fno-PIE -fno-PIC -march=core2 -ffunction-sections -fdata-sections

ifeq ($(DEBUG),false)
	CFLAGS += -nostdlib
else
	CFLAGS += -DDEBUG=true
endif

.PHONY: clean checkgccversion noelfver

all : checkgccversion gtk-opengl xlib-opengl sdl2-opengl

noelfver:
	make -C noelfver

checkgccversion :
ifneq ($(shell expr `gcc -dumpversion`),8.3.0)
	$(error GCC version must be 8.3.0)
endif

packer : vondehi/vondehi.asm 
	cd vondehi; nasm -fbin -o vondehi vondehi.asm

shader.h : shader.frag Makefile
	mono ./shader_minifier.exe --preserve-externals shader.frag -o shader.h

gtk-opengl.elf : gtk-opengl.c shader.h Makefile
	gcc -o $@ $< `pkg-config --cflags gtk+-3.0` -lGL -lgtk-3 -lgdk-3 -lgobject-2.0 $(CFLAGS)

xlib-opengl.elf : xlib-opengl.c shader.h Makefile
	gcc -o $@ $<  -lX11 -lGL -lXrandr $(CFLAGS)

sdl2-opengl.elf : sdl2-opengl.c shader.h Makefile
	gcc -o $@ $<  -lSDL2 -lGL $(CFLAGS)

gtk-opengl : gtk-opengl_opt.elf.packed
	mv $< $@

xlib-opengl : xlib-opengl_opt.elf.packed
	mv $< $@

sdl2-opengl : sdl2-opengl_opt.elf.packed
	mv $< $@

index.html.inc : index.html
	cat index.html | xxd -i > index.html.inc
	echo ", 0" >> index.html.inc

#all the rest of these rules just takes a compiled elf file and generates a packed version of it with vondehi
%_opt.elf : %.elf Makefile noelfver
	cp $< $@
	strip $@
	strip -R .note -R .comment -R .eh_frame -R .eh_frame_hdr -R .note.gnu.build-id -R .got -R .got.plt -R .gnu.version -R .shstrtab -R .gnu.version_r -R .gnu.hash $@
	./noelfver/noelfver $@ > $@.nover
	mv $@.nover $@
	#remove section header
	./Section-Header-Stripper/section-stripper.py $@

	#clear out useless bits
	sed -i 's/_edata/\x00\x00\x00\x00\x00\x00/g' $@;
	sed -i 's/__bss_start/\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00/g' $@;
	sed -i 's/_end/\x00\x00\x00\x00/g' $@;
	sed -i 's/GLIBC_2\.2\.5/\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00/g' $@;

	chmod +x $@

%.xz : % Makefile
	-rm $@
	lzma --format=lzma -9 --extreme --lzma1=preset=9,lc=0,lp=0,pb=0,nice=40,depth=32,dict=16384 --keep --stdout $< > $@

%.packed : %.xz packer Makefile
	cat ./vondehi/vondehi $< > $@
	chmod +x $@
	wc -c $@

clean :
	-rm *.elf shader.h gtk-opengl xlib-opengl sdl2-opengl
