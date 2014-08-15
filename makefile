OUT=ebr_editor

all:
	make -C src
	cp src/$(OUT) $(OUT)
	strip --strip-unneeded $(OUT)
clean:
	make -C src clean

