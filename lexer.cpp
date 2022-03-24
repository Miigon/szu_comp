#include <iostream>
#include "lexer.hpp"
#include "typeutils.hpp"

Lexer::Lexer(std::istream &is)
: mis(is), mline(1), mcol(0) {}

int Lexer::next() {
	if(mis.eof()) return EOF;
	mcol++;
	return mis.get();
}

int Lexer::peek() {
	if(mis.eof()) return EOF;
	return mis.peek();
}

void Lexer::skip(int n) {
	mcol++;
	mis.ignore(n);
}

std::vector<Token> Lexer::tokenize() {
	std::vector<Token> v;
	do {
		v.push_back(nextToken());
	} while(v.back().getTokenType() != Token::bad && v.back().getTokenType() != Token::eof);

	return std::move(v);
}

Token Lexer::nextToken() {
	int ch;
	do { // skip whitespaces
		ch = next();
		if(ch == '\n') {
			mcol = 0;
			mline++;
		}
	} while(iswhitespace(ch));

	if(ch == EOF) return Token(Token::eof, "eof");

	int col_ch = mcol;

	// operator
	switch(ch) {
		case ':':
			if(peek() == '=') {
				skip();
				return Token(Token::assignOp, ":=", mline, col_ch);
			} else {
				goto bad;
			}
		case '+': return Token(Token::plusOp, "+", mline, col_ch);
		case '*': return Token(Token::multiplyOp, "*", mline, col_ch);
		case ';': return Token(Token::delim, ";", mline, col_ch);
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
		return Token(Token::numLiteral, numliteral, mline, col_ch);
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
		return Token(Token::identifier, identifier, mline, col_ch);
	}

	goto bad;

	bad:
	return Token(Token::bad, std::string("") + (char)ch, mline, mcol);
}