GIT_REVISION = $(shell git rev-parse HEAD | tr -d '\n')
TAG_VERSION?=$(shell git tag --points-at | sort -Vr | head -n1)
DATE_COMPILED?=$(shell date +'%Y-%m-%d')
COMMON_LDFLAGS = -s -w -X 'mlmodel-tflite/config.Version=${TAG_VERSION}' -X 'mlmodel-tflite/config.GitRevision=${GIT_REVISION}' -X 'mlmodel-tflite/config.DateCompiled=${DATE_COMPILED}'
LDFLAGS = -ldflags "-extld=$(shell pwd)/etc/ld_wrapper.sh $(COMMON_LDFLAGS)"
BIN_OUTPUT_PATH = bin
TOOL_BIN = bin/gotools/$(shell uname -s)-$(shell uname -m)
UNAME_S ?= $(shell uname -s)

build:
	rm -f tflite_cpu
	LIBRARY_PATH= GOOS=darwin GOARCH=arm64 CGO_ENABLED=1 CGO_LDFLAGS= \
		 go build -v -o tflite_cpu main.go 
	install_name_tool -add_rpath @executable_path/tflitecpu/darwin/arm64/ tflite_cpu 
	install_name_tool -change /opt/homebrew/opt/tensorflowlite/lib/libtensorflowlite_c.dylib @rpath/libtensorflowlite_c.dylib tflite_cpu

module.tar.gz: build
	mkdir -p $(BIN_OUTPUT_PATH)
	rm -f $(BIN_OUTPUT_PATH)/module.tar.gz
	tar czf $(BIN_OUTPUT_PATH)/module.tar.gz $(BIN_OUTPUT_PATH)/tflite_cpu

static_tflite_cpu: main.go
	go build -ldflags="-extldflags=-static $(COMMON_LDFLAGS)" -o bin/static/tflite_cpu-$(shell go env GOARCH) main.go

setup:
	if [ "$(UNAME_S)" = "Linux" ]; then \
		sudo apt install -y libnlopt-dev libjpeg-dev pkg-config; \
	fi

clean:
	rm -rf $(BIN_OUTPUT_PATH)/tflite_cpu $(BIN_OUTPUT_PATH)/module.tar.gz tflite_cpu

gofmt:
	gofmt -w -s .

lint: gofmt
	go mod tidy

update-rdk:
	go get go.viam.com/rdk@latest
	go mod tidy
