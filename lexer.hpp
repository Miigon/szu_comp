#pragma once
#include <string>
#include <stdexcept>
#include <vector>

class Token {
	public:
	enum TokenType{
		bad, // bad token, should be treated as an error
		eof,
		identifier,
		numLiteral,
		plusOp,
		assignOp,
		multiplyOp,
		delim,
	};
	
	static const char *tokenTypeToStr(TokenType t) {
		switch(t) {
			case bad: return "bad";
			case eof: return "eof";
			case identifier: return "identifier";
			case numLiteral: return "numLiteral";
			case assignOp: return "assignOp";
			case plusOp: return "plusOp";
			case multiplyOp: return "multiplyOp";
			case delim: return "delim";
			default: return "unknown";
		}
	}

	Token(TokenType type, const std::string &str)
	: mTokenType(type), mStrData(str) {}

	TokenType getTokenType() const { return mTokenType; }
	std::string getStringData() const {
		return mStrData;
	}

	private:	
	TokenType mTokenType;
	std::string mStrData;
};

class Lexer {
	public:
	Lexer(const std::string &str);
	std::vector<Token> tokenize();

	private:
	std::string mstr;
	int mpos;

	int next(int n = 1);	// next nth character
	int peek(int n = 1);	// next nth character, without advancing pointer
	void skip(int n = 1);	// skip n characters

	Token nextToken();
};
