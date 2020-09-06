all: build

build: binaries
	@echo "Building"
	gcc shell.c -o shell

binaries:
	@echo "Building ls..."
	gcc ls.c -o ls
	@echo "Building cat..."
	gcc cat.c -o cat
	@echo "Building grep..."
	gcc grep.c -o grep
	@echo "Building pwd..."
	gcc pwd.c -o pwd
	@echo "Building mkdir..."
	gcc mkdir.c -o mkdir
	@echo "Building mv..."
	gcc mv.c -o mv
	@echo "Building cp..."
	gcc cp.c -o cp
	@echo "Building rm..."
	gcc rm.c -o rm
	@echo "Building chmod..."
	gcc chmod.c -o chmod

clean:
	@echo "Cleaning"
	rm ls
	rm cat
	rm grep
	rm pwd
	rm mkdir
	rm mv
	rm cp
	rm rm
	rm chmod
	rm shell
