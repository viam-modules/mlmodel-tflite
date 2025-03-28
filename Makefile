.PHONY: format
format: src/*.cpp src/*.hpp test/*.cpp
	ls src/*.cpp src/*.hpp test/*.cpp | xargs clang-format -i --style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100}"

.PHONY: tflite_cpu
tflite_cpu: build/Release/tflite_cpu

build/Release/tflite_cpu: src/*
	rm -rf build-conan/ && \
	conan install . --output-folder=build-conan --build=missing -o "&:shared=False" && \
	conan build . -o "&:shared=False"

.PHONY: setup
setup:
	./bin/setup.sh

module.tar.gz: build/Release/tflite_cpu
	tar -cvf module.tar build/Release/tflite_cpu && \
	gzip -o module.tar

.PHONY: lint
lint:
	./bin/run-clang-format.sh
