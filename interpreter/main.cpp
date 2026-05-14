// main.cpp
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic/SemanticAnalyzer.h"
#include "runtime/Interpreter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>

int main(int argc, char* argv[]) {
    // Handle command line argument
    std::string filepath = "program.txt"; // default
    if (argc >= 2) {
        filepath = argv[1];
    }

    // Read source file
    std::ifstream input(filepath);
    if (!input.is_open()) {
        std::cerr << "Error: Could not open file '" << filepath << "'\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string source = buffer.str();

    try {
        // Lexical analysis
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        // Parsing
        Parser parser(tokens);
        auto program = parser.parseProgram();

        // Semantic analysis
        // SemanticAnalyzer semanticAnalyzer;
        // semanticAnalyzer.analyze(program.get());

        // Interpretation
        Interpreter interpreter;
        program->execute(interpreter);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
