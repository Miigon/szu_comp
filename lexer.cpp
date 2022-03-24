#include <iostream>
#include "lexer.hpp"
#include "typeutils.hpp"

Lexer::Lexer(const std::string &str)
: mstr(str), mpos(-1) {}

int Lexer::next(int n) {
	if(mpos+n >= mstr.size()) return EOF;
	return mstr[mpos+=n];
}

int Lexer::peek(int n) {
	if(mpos+n >= mstr.size()) return EOF;
	return mstr[mpos+n];
}

void Lexer::skip(int n) {
	if(mpos+n >= mstr.size()) mpos = mstr.size();
	mpos += n;
}

std::vector<Token> Lexer::tokenize() {
	std::vector<Token> v;
	do {
		v.push_back(nextToken());
	} while(v.back().getTokenType() != Token::bad && v.back().getTokenType() != Token::eof);

	return std::move(v);
}

Token Lexer::nextToken() {
	int ch = next();
	while(iswhitespace(ch)) { // skip whitespaces
		ch = next();
	}

	if(ch == EOF) return Token(Token::eof, "eof");

	// operator
	switch(ch) {
		case ':':
			if(peek() == '=') {
				skip();
				return Token(Token::assignOp, ":=");
			} else {
				goto bad;
			}
		case '+': return Token(Token::plusOp, "+");
		case '*': return Token(Token::multiplyOp, "*");
		case ';': return Token(Token::delim, ";");
	}
	
	// number literal
	if(isnumber(ch)) {
		std::string numliteral;
		numliteral += ch;
		while((ch = peek()) != EOF) {
			if(isnumber(ch)) {
				skip();
				numliteral += ch;
			} else {
				break;
			}
		}
		return Token(Token::numLiteral, numliteral);
	}

	// identifier
	if(isidentbegin(ch)) {
		std::string identifier;
		identifier += ch;
		while((ch = peek()) != EOF) {
			if(isident(ch)) {
				skip();
				identifier += ch;
			} else {
				break;
			}
		}
		return Token(Token::identifier, identifier);
	}

	goto bad;

	bad:
	return Token(Token::bad, std::string("")+(char)ch);
}