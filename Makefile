.PHONY: run all build release test install clean

run: build
	./build/axigate -v
	
all: test

build:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
	cd build && make

release:
	mkdir -p build
	cd build && cmake -G "Unix Makefiles" ..
	cd build && make

test: build
	./build/axigate_test

install: release
	cd build && sudo make install

clean:
	rm -rf build
