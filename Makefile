make: build
	./build/main

build:
	mkdir build
	cmake -B build
	cd ./build && make

clean:
	rm -rf build