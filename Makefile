
all : gtk-webkit

.PHONY: clean

packer : vondehi/vondehi.asm 
	cd vondehi; nasm -fbin -o vondehi vondehi.asm

gtk-webkit.elf : gtk-webkit.c Makefile
	gcc -o $@ $< `pkg-config --cflags webkit2gtk-4.0` -lgobject-2.0 -lgtk-3 -lwebkit2gtk-4.0 -no-pie -fno-plt -Os -std=gnu11 -nostartfiles -nostdlib

gtk-webkit : gtk-webkit_opt.elf.packed
	cp $< $@

%_opt.elf : %.elf Makefile
	cp $< $@
	strip $@
	strip -R .note -R .comment -R .eh_frame -R .eh_frame_hdr -R .note.gnu.build-id -R .got -R .got.plt -R .gnu.version -R .rela.dyn -R .shstrtab $@
	#remove section header
	./section-stripper/section-stripper.py $@

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
	rm *.xz *.elf *.packed gtk-webkit
