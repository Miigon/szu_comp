#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char **argv) {
	if(argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
		return 1;
	}

	std::ifstream is(argv[1]);
	std::cout << "# input file: " << argv[1] << std::endl;

	Lexer lex(is);
	std::vector<Token> tokens = lex.tokenize();

	// check for bad token
	const Token &lastToken = tokens.back();
	if(lastToken.getTokenType() == Token::bad) {
		std::cerr << "Bad token at line " << lastToken.getLine()
					<< ", column " << lastToken.getColumn() << std::endl;
		return -1;
	}

	Parser p(tokens);
	Parser::Grammar gram = p.parseGrammar();
	

	std::cout << "expanded grammar:" << std::endl;
	std::cout << "G[" << gram.startSymbol << "];" << std::endl;
	for(const Parser::Production &p : gram.prods) {
		for(const Token &s : p.leftSymbols) {
			if(s.getTokenType() == Token::terminalSymbol) {
				std::cout << "'" << s.getStringData() << "' ";
			} else {
				std::cout << s.getStringData() << " ";
			}
		}
		std::cout << "-> ";
		for(const Token &s : p.rightSymbols) {
			if(s.getTokenType() == Token::terminalSymbol) {
				std::cout << "'" << s.getStringData() << "' ";
			} else {
				std::cout << s.getStringData() << " ";
			}
		}
		std::cout << ";  ( " << Parser::grammarTypeStr(p.detectGrammarType()) << " )" << std::endl;
	}
	std::cout << std::endl << "grammar type: " << Parser::grammarTypeStr(gram.detectGrammarType()) << std::endl;

	return 0;
}