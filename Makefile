all:
	make -C src

clean:
	make -C src clean
	make -C src/lib/simplefilewatcher/build/gmake clean
