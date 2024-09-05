format: src/*.cpp src/*.hpp test/*.cpp
	ls src/*.cpp src/*.hpp test/*.cpp | xargs clang-format -i --style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100}"

tflite_cpu: src/*
	rm -rf build/ && \
	mkdir build && \
	cd build && \
	cmake -G Ninja .. && \
	ninja -v all -j 4 && \
	mv tflite_cpu ../
