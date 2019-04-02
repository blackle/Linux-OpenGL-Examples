
all : gtk-webkit gtk-opengl xlib-opengl

.PHONY: clean

packer : vondehi/vondehi.asm 
	cd vondehi; nasm -fbin -o vondehi vondehi.asm

shader.h : shader.frag Makefile
	mono ./shader_minifier.exe --preserve-externals shader.frag -o shader.h

# not using `pkg-config --libs` here because it will include too many libs
gtk-webkit.elf : gtk-webkit.c index.html.inc Makefile
	gcc -o $@ $< `pkg-config --cflags webkit2gtk-4.0` -lgobject-2.0 -lgtk-3 -lwebkit2gtk-4.0 -no-pie -fno-plt -Os -std=gnu11 -nostartfiles -nostdlib

gtk-opengl.elf : gtk-opengl.c shader.h Makefile
	gcc -o $@ $< `pkg-config --cflags gtk+-3.0` -lglib-2.0 -lGL -lgtk-3 -lgdk-3 -lgobject-2.0 -no-pie -fno-plt -Os -std=gnu11 -nostartfiles -nostdlib

xlib-opengl.elf : xlib-opengl.c shader.h Makefile
	gcc -o $@ $<  -lX11 -lGL -lcairo -lXrandr -no-pie -fno-plt -Os -std=gnu11 -nostartfiles -nostdlib

gtk-webkit : gtk-webkit_opt.elf.packed
	mv $< $@

gtk-opengl : gtk-opengl_opt.elf.packed
	mv $< $@

xlib-opengl : xlib-opengl_opt.elf.packed
	mv $< $@

index.html.inc : index.html
	cat index.html | xxd -i > index.html.inc
	echo ", 0" >> index.html.inc

#all the rest of these rules just takes a compiled elf file and generates a packed version of it with vondehi
%_opt.elf : %.elf Makefile
	cp $< $@
	strip $@
	strip -R .note -R .comment -R .eh_frame -R .eh_frame_hdr -R .note.gnu.build-id -R .got -R .got.plt -R .gnu.version -R .rela.dyn -R .shstrtab $@
	#remove section header
	./Section-Header-Stripper/section-stripper.py $@

	#clear out useless bits
	sed -i 's/_edata/\x00\x00\x00\x00\x00\x00/g' $@;
	sed -i 's/__bss_start/\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00/g' $@;
	sed -i 's/_end/\x00\x00\x00\x00/g' $@;

	wc -c $@
	chmod +x $@

%.xz : % Makefile
	-rm $@
	lzma --format=lzma -9 --extreme --lzma1=preset=9,lc=1,lp=0,pb=1,nice=270,depth=64,dict=16384 --keep --stdout $< > $@
	wc -c $@

%.packed : %.xz packer Makefile
	cat ./vondehi/vondehi $< > $@
	chmod +x $@
	wc -c $@

clean :
	-rm *.elf shader.h gtk-webkit gtk-opengl xlib-opengl
