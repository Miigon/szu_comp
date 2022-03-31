#include "parser.hpp"
#include <stdexcept>
#include <algorithm>
#include <sstream>

Parser::GrammarType Parser::Production::detectGrammarType() const {
	using GT = Parser::GrammarType;
	
	int nLeftSideVN = 0, nLeftSideVT = 0;
	for(const Token &t : leftSymbols) {
		if(t.getTokenType() == Token::nonterminalSymbol) nLeftSideVN++;
		else if(t.getTokenType() == Token::terminalSymbol) nLeftSideVT++;
		else return GT::INVALID;
	}
	int nRightSideVN = 0, nRightSideVT = 0;
	for(const Token &t : rightSymbols) {
		if(t.getTokenType() == Token::nonterminalSymbol) nRightSideVN++;
		else if(t.getTokenType() == Token::terminalSymbol) nRightSideVT++;
		else if(t.getTokenType() == Token::epsilon) nRightSideVT++;
		else return GT::INVALID;
	}
	if(nLeftSideVN < 1) return GT::INVALID;
	if(nLeftSideVN == 1 && nLeftSideVT == 0) { // A ->
		if(nRightSideVN == 0) { // A -> abc
			if(rightSymbols.front().getTokenType() == Token::epsilon) {
				// A -> epsilon
				return GT::PSG_0;
			}
			return GT::RG_3_LINEAR;
		} else if (nRightSideVN == 1) {
			// extended definition of RG_3, can have multiple VTs.
			if(nRightSideVT == 0) {
				// A -> B
				return GT::RG_3_LINEAR;
			} else if(rightSymbols.back().getTokenType() == Token::nonterminalSymbol) {
				// A -> aB
				return GT::RG_3_RIGHT_LINEAR;
			} else if(rightSymbols.front().getTokenType() == Token::nonterminalSymbol) {
				// A -> Ba
				return GT::RG_3_LEFT_LINEAR;
			}
		}
		// strict definition of CFG, prohibits epsilon-production.
		return GT::CFG_2;
	} else if (leftSymbols.size() <= rightSymbols.size()) {
		// nLeftSideVN > 1
		return GT::CSG_1;
	}
	return GT::PSG_0;
}

Parser::GrammarType Parser::Grammar::detectGrammarType() {
	using GT = Parser::GrammarType;
	bool exist[(int)GT::RG_3_LINEAR+1] = {false};

	for(const Production &p : prods) {
		GT type = p.detectGrammarType();
		if(
			p.leftSymbols.size() == 1 && p.leftSymbols.front().getStringData() == startSymbol
			&& p.rightSymbols.size() == 1 && p.rightSymbols.front().getTokenType() == Token::epsilon
		) {
			// special case for CSG_1:
			// S -> epsilon and start symbol S is never on the right side of any production.
			bool found_S_on_right_side = false;
			for(const Production &pp : prods) {
				if(&pp == &p) continue;
				for(const Token &t : pp.rightSymbols) {
					if(t.getStringData() == startSymbol) {
						found_S_on_right_side = true;
						break;
					}
				}
				if(found_S_on_right_side) break;
			}
			type = !found_S_on_right_side ? GT::CSG_1 : type;
		}
		exist[(int)type] = true;
	}

	if(exist[(int)GT::INVALID]) return GT::INVALID;
	if(exist[(int)GT::PSG_0]) return GT::PSG_0;
	if(exist[(int)GT::CSG_1]) return GT::CSG_1;
	if(exist[(int)GT::CFG_2]) return GT::CFG_2;

	if(exist[(int)GT::RG_3_LEFT_LINEAR] && exist[(int)GT::RG_3_RIGHT_LINEAR]) {
		return GT::RG_3; // ordinary RG_3
	}

	if(exist[(int)GT::RG_3_LINEAR]) { // E -> 'a'
		if(exist[(int)GT::RG_3_LEFT_LINEAR]) { // linear + left linear
			return GT::RG_3_LEFT_LINEAR;
		} else if(exist[(int)GT::RG_3_RIGHT_LINEAR]) { // linear + right linear
			return GT::RG_3_RIGHT_LINEAR;
		} else { // all linear
			return GT::RG_3_LINEAR;
		}
	}

	return GT::INVALID;
}

const Token &Parser::peek() {
	return mTokens[mi];
}

void Parser::skip() {
	mi++;
}

void Parser::badToken() {
	const Token &badToken = peek();
	std::stringstream ss;
	ss << "unexpected token " << Token::tokenTypeToStr(badToken.getTokenType())
	   << " near " << badToken.getLine() << ":" << badToken.getColumn();
	throw std::runtime_error(ss.str());
}

void Parser::expectToken(Token::TokenType type) {
	if(peek().getTokenType() != type) {
		badToken();
	}
}

void Parser::expectTokenData(const std::string &data) {
	if(peek().getStringData() != data) {
		badToken();
	}
}

Parser::Grammar Parser::parseGrammar() {
	Grammar gram;

	// G[S];
	expectToken(Token::nonterminalSymbol);
	expectTokenData("G"); skip();
	expectToken(Token::leftBracket); skip();
	expectToken(Token::nonterminalSymbol);
	gram.startSymbol = peek().getStringData();
	skip();
	expectToken(Token::rightBracket); skip();
	expectToken(Token::delim); skip();

	while(peek().getTokenType() != Token::eof) {
		std::vector<Token> left = std::move(parseProdLeftSide());

		// expect ->
		if(peek().getTokenType() == Token::rightArrow) {
			skip();
		} else {
			badToken();
		}

		while(true) {
			std::vector<Token> right = std::move(parseProdRightSide());
			gram.prods.push_back(Production{left, right});
			if(peek().getTokenType() == Token::delim) {
				skip(); // skip delim
				break;
			}
			skip(); // skip alternative
		}
	}
	
	return std::move(gram);
}

std::vector<Token> Parser::parseProdLeftSide() {
	std::vector<Token> left;

	// left side of production
	Token::TokenType tokentype;
	while((tokentype = peek().getTokenType()) != Token::eof) {
		if(tokentype == Token::terminalSymbol || tokentype == Token::nonterminalSymbol) {
			left.push_back(peek());
		} else if(tokentype == Token::rightArrow && left.size() != 0) {
			break;
		} else {
			badToken();
		}
		skip();
	}

	return std::move(left);
}

std::vector<Token> Parser::parseProdRightSide() {
	std::vector<Token> right;

	// right side of production
	Token::TokenType tokentype;
	while((tokentype = peek().getTokenType()) != Token::eof) {
		if(tokentype == Token::terminalSymbol || tokentype == Token::nonterminalSymbol) {
			right.push_back(peek());
		} else if(tokentype == Token::epsilon && right.size() == 0){ // -> epsilon
			right.push_back(peek());
			skip();
			break;
		} else if((tokentype == Token::delim || tokentype == Token::alternative) && right.size() != 0) {
			break;
		} else {
			badToken();
		}
		skip();
	}

	return std::move(right);
}