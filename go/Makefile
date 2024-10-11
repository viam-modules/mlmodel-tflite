GIT_REVISION = $(shell git rev-parse HEAD | tr -d '\n')
TAG_VERSION?=$(shell git tag --points-at | sort -Vr | head -n1)
DATE_COMPILED?=$(shell date +'%Y-%m-%d')
COMMON_LDFLAGS = -s -w -X 'mlmodel-tflite/config.Version=${TAG_VERSION}' -X 'mlmodel-tflite/config.GitRevision=${GIT_REVISION}' -X 'mlmodel-tflite/config.DateCompiled=${DATE_COMPILED}'
LDFLAGS = -ldflags "-extld=$(shell pwd)/etc/ld_wrapper.sh $(COMMON_LDFLAGS)"
BIN_OUTPUT_PATH = bin
TOOL_BIN = bin/gotools/$(shell uname -s)-$(shell uname -m)
UNAME_S ?= $(shell uname -s)

MOD_ARCH := $(shell uname -m)
MOD_OS := $(shell uname -s)

module.tar.gz: tflite_cpu
ifeq ($(MOD_OS),Darwin)
ifeq ($(MOD_ARCH),x86_64)
	@echo "Unsupported OS: $(MOD_OS) or architecture: $(MOD_ARCH)"
else ifeq ($(MOD_ARCH),arm64)
	tar -czf $@ tflite_cpu tflitecpu/darwin/arm64/libtensorflowlite_c.dylib
endif
else ifeq ($(MOD_OS),Linux)
ifeq ($(MOD_ARCH),x86_64)
	tar -czf $@ tflite_cpu tflitecpu/linux/amd64/libtensorflowlite_c.so
else ifeq ($(MOD_ARCH),arm64)
	tar -czf $@ tflite_cpu tflitecpu/linux/arm64/libtensorflowlite_c.so
else ifeq ($(MOD_ARCH),aarch64)
	tar -czf $@ tflite_cpu tflitecpu/linux/arm64/libtensorflowlite_c.so
endif
else
    @echo "Unsupported OS: $(MOD_OS) or architecture: $(MOD_ARCH)"
endif

%: TFLITE_LIB_PATH = $(shell otool -L tflite_cpu | grep libtensorflowlite_c.dylib | awk '{print $$1}')

build:
	rm -rf tflite_cpu
	go build -v -o tflite_cpu main.go

tflite_cpu: build
ifeq ($(MOD_OS),Darwin)
ifeq ($(MOD_ARCH),x86_64)
	@echo "Unsupported OS: $(MOD_OS) or architecture: $(MOD_ARCH)"
else ifeq ($(MOD_ARCH),arm64)
	install_name_tool -add_rpath @executable_path/tflitecpu/darwin/arm64/ tflite_cpu 
	@if [ -z "$(TFLITE_LIB_PATH)" ]; then \
		echo "libtensorflowlite_c.dylib not found in the binary tflite_cpu."; \
		exit 1; \
	fi
	@if [ ! -f "$(TFLITE_LIB_PATH)" ] ; then \
		echo "$(TFLITE_LIB_PATH) not found."; \
		exit 1; \
	fi
	install_name_tool -change $(TFLITE_LIB_PATH) @rpath/libtensorflowlite_c.dylib tflite_cpu
endif
else ifeq ($(MOD_OS),Linux)
ifeq ($(MOD_ARCH),x86_64)
	patchelf --set-rpath '$$ORIGIN/tflitecpu/linux/amd64' tflite_cpu
else ifeq ($(MOD_ARCH),arm64)
	patchelf --set-rpath '$$ORIGIN/tflitecpu/linux/arm64' tflite_cpu
else ifeq ($(MOD_ARCH),aarch64)
	patchelf --set-rpath '$$ORIGIN/tflitecpu/linux/arm64' tflite_cpu
endif
else
    @echo "Unsupported OS: $(MOD_OS) or architecture: $(MOD_ARCH)"
endif

setup:
ifeq ($(MOD_OS),Darwin)
	brew install tensorflowlite;
else ifeq ($(MOD_OS),Linux)
	sudo apt install -y vim libjpeg-dev pkg-config patchelf; 
endif

clean:
	rm -rf tflite_cpu module.tar.gz

gofmt:
	gofmt -w -s .

lint: gofmt
	go mod tidy

update-rdk:
	go get go.viam.com/rdk@latest
	go mod tidy
