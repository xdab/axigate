.PHONY: run all build release test install clean update

run: build
	./build/axigate -c sample.conf
	
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
	
update:
	git pull
	git submodule update --init --recursive