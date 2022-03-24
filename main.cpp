#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"

int main(int argc, char **argv) {
	if(argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
		return 1;
	}

	std::ifstream t(argv[1]);
	std::stringstream buffer; 
	buffer << t.rdbuf();

	Lexer lex(buffer.str());
	std::vector<Token> tokens = lex.tokenize();

	for(const Token &t : tokens ) {
		std::cout
			<< "(" << Token::tokenTypeToStr(t.getTokenType())
			<< ", " << t.getStringData() << ")" << std::endl;
	}

	return 0;
}