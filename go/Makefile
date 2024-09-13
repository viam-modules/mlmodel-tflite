
BIN_OUTPUT_PATH = bin
TOOL_BIN = bin/gotools/$(shell uname -s)-$(shell uname -m)
UNAME_S ?= $(shell uname -s)

build:
	mkdir -p $(BIN_OUTPUT_PATH)
	rm -f $(BIN_OUTPUT_PATH)/tflitecpu
	go build $(LDFLAGS) -o $(BIN_OUTPUT_PATH)/tflitecpu main.go

module.tar.gz: build
	mkdir -p $(BIN_OUTPUT_PATH)
	rm -f $(BIN_OUTPUT_PATH)/module.tar.gz
	tar czf $(BIN_OUTPUT_PATH)/module.tar.gz $(BIN_OUTPUT_PATH)/tflitecpu

tflitecpu: *.go 
	go build -o tflitecpu *.go

setup:
	if [ "$(UNAME_S)" = "Linux" ]; then \
		sudo apt install -y libnlopt-dev libjpeg-dev pkg-config; \
	fi

clean:
	rm -rf $(BIN_OUTPUT_PATH)/tflitecpu $(BIN_OUTPUT_PATH)/module.tar.gz tflitecpu

gofmt:
	gofmt -w -s .

lint: gofmt
	go mod tidy

update-rdk:
	go get go.viam.com/rdk@latest
	go mod tidy
