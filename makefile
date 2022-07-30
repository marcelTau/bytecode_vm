
build_dir	= build
exe_name 	= TODO_ChangeExecutableName

# name of the test target in the build makefile
test_name 	= Tests

help:
	@echo "USAGE:"
	@echo -e "    - build -- to build the code"
	@echo -e "    - test -- run tests"
	@echo -e "    - run -- runs the program with no arguments"
	@echo -e "    - compile_commands -- build the compile_commands.json file"

compile_commands:
	@cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .

build:
	@cmake -B $(build_dir) -S .
	@make $(exe_name) -C $(build_dir)

test:
	@cmake -B $(build_dir) -S .
	@make $(test_name) -C $(build_dir)
	./build/test/$(test_name) --gtest_color=yes

ctest:
	cmake --build $(build_dir) --config Debug
	ctest --test-dir build -C Debug

coverage: build test
	@gcovr -f src -f include
	# @lcov --capture --directory . --output-file coverage.info
	@gcovr --xml-pretty -f src -f include > coverage.xml


run: build
	./$(build_dir)/$(exe_name)

clean:
	make clean -C build

.PHONY: build compile_commands test run help clean
