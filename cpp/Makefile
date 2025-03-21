format: src/*.cpp src/*.hpp test/*.cpp
	ls src/*.cpp src/*.hpp test/*.cpp | xargs clang-format -i --style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100}"

tflite_cpu: src/*
	rm -rf build-conan/ && \
	conan install . --output-folder=build-conan --build=missing -o "*:shared=False" && \
	conan create . -o "&:shared=False" -o "*:shared=False"

TAG_VERSION?=latest
tflite_cpu-appimage: export TAG_NAME = ${TAG_VERSION}
tflite_cpu-appimage: tflite_cpu 
	cd packaging/appimages && \
	mkdir -p deploy && \
	rm -f deploy/tflite_cpu* && \
	appimage-builder --recipe viam-tflite-cpu-aarch64.yml
	cp ./packaging/appimages/*-aarch64.AppImage ./packaging/appimages/deploy/
	chmod a+x ./packaging/appimages/deploy/*
