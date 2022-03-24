#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"

int main(int argc, char **argv) {
	if(argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
		return 1;
	}

	std::ifstream is(argv[1]);

	Lexer lex(is);
	std::vector<Token> tokens = lex.tokenize();

	for(const Token &t : tokens ) {
		if(t.getTokenType() == Token::eof) break;
		std::cout
			<< t.getLine() << ":" << t.getColumn() << "\t"
			<< "(" << Token::tokenTypeToStr(t.getTokenType())
			<< ", " << t.getStringData() << ")"
			<< std::endl;
	}

	return 0;
}