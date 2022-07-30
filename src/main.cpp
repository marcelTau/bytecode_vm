#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "header.h"
#include "chunk.h"
#include "vm.h"

static int repl() {
    VM vm;
    while (true) {
        fmt::print("> ");

        std::string input;

        std::cin >> input;
        vm.interpret(input);
    }

    return 0;
}

static int runFile(const std::filesystem::path& path) {
    if (not std::filesystem::exists(path)) {
        fmt::print(stderr, "The file: {} does not exists or can not be opened\n", path.string());
        return 1;
    }

    VM vm;
    std::ifstream ifs(path);

    if (not ifs.is_open()) {
        fmt::print(stderr, "The file: {} does not exists or can not be opened\n", path.string());
        return 1;
    }

    std::string source;
    source.reserve(std::filesystem::file_size(path));
    source.assign( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>() ));

    auto result = vm.interpret(source);

    if (result == InterpretResult::CompileError) {
        return 1;
    }

    if (result == InterpretResult::RuntimeError) {
        return 2;
    }

    return 0;
}

int main(int argc, char *argv[]) {
#ifdef DEBUG_TRACE_EXECUTION
    fmt::print("===== DEBUG MODE =====\n");
#endif

    VM vm;

    if (argc == 1) {
        return repl();
    } else if (argc == 2) {
        return runFile(argv[1]);
    } else {
        fmt::print(stderr, "Usage: Bytecode-VM [path]");
        std::exit(84);
    }
}
