#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

class Token {
	public:
	enum TokenType{
		bad, // bad token, should be treated as an error
		eof,
		nonterminalSymbol,	// abc123
		terminalSymbol,		// 'abc123'
		epsilon,			// epsilon
		alternative,		// |
		rightArrow,			// ->
		delim,				// ;
	};
	
	static const char *tokenTypeToStr(TokenType t) {
		switch(t) {
			case bad: return "bad";
			case eof: return "eof";
			case nonterminalSymbol: return "nonterminal";
			case terminalSymbol: return "terminal";
			case epsilon: return "epsilon";
			case alternative: return "alternative";
			case rightArrow: return "rightArrow";
			case delim: return "delim";
			default: return "unknown";
		}
	}

	Token(TokenType type, const std::string &str, int line = 0, int col = 0)
	: mTokenType(type), mStrData(str), mline(line), mcol(col) {}

	TokenType getTokenType() const { return mTokenType; }
	std::string getStringData() const {
		return mStrData;
	}

	int getLine() const { return mline; }
	int getColumn() const { return mcol; }

	private:
	int mline, mcol;
	TokenType mTokenType;
	std::string mStrData;
};

class Lexer {
	public:
	Lexer(std::istream &is);
	std::vector<Token> tokenize();

	private:
	std::istream &mis;
	int mline, mcol;

	int next();	// next character
	int peek();	// next character, without advancing pointer
	void skip(int n = 1);	// skip n characters

	Token nextToken();
};
