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
	if(ch == '/' && peek() == '/') { // comment
		while(peek() != '\n') skip();
		return std::move(nextToken());
	}

	int col_ch = mcol;

	bool escaped = false;
	// operator
	switch(ch) {
		case '-':
			if(peek() == '>') {
				skip();
				return Token(Token::rightArrow, "->", mline, col_ch);
			} else {
				goto bad;
			}
		case '|': return Token(Token::alternative, "|", mline, col_ch);
		case '[': return Token(Token::leftBracket, "[", mline, col_ch);
		case ']': return Token(Token::rightBracket, "]", mline, col_ch);
		case '\'':
			escaped = true;
			ch = next();
			break;
		case ';': return Token(Token::delim, ";", mline, col_ch);
	}

	// nonterminal/terminal symbols
	if(issymbol(ch)) {
		std::string identifier;
		identifier += ch;
		while((ch = peek()) != EOF) {
			if(issymbol(ch)) {
				skip();
				identifier += ch;
			} else {
				break;
			}
		}
		if(escaped) {
			if(next() != '\'') goto bad;
			return Token(Token::terminalSymbol, identifier, mline, col_ch);
		}
		if(identifier == "epsilon") {
			return Token(Token::epsilon, "epsilon", mline, col_ch);
		}
		return Token(Token::nonterminalSymbol, identifier, mline, col_ch);
	}

	goto bad;

	bad:
	return Token(Token::bad, std::string("") + (char)ch, mline, mcol);
}