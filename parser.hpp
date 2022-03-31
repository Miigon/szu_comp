#pragma once
#include <vector>
#include "lexer.hpp"

class Parser {
	public:
	// production, with alternatives expanded into multiple productions.
	enum class GrammarType {
		INVALID,
		PSG_0,
		CSG_1,
		CFG_2,
		RG_3_LEFT_LINEAR, RG_3_RIGHT_LINEAR, RG_3_LINEAR
	};

	struct Production {
		std::vector<Token> leftSymbols;
		std::vector<Token> rightSymbols;
		Parser::GrammarType detectGrammarType() const;
	};

	struct Grammar {
		std::string startSymbol;
		std::vector<Production> prods;
		GrammarType detectGrammarType();
	};
	static const char *grammarTypeStr(GrammarType gt) {
		switch(gt) {
			case GrammarType::PSG_0: return "phase structure-0";
			case GrammarType::CSG_1: return "context sensitive-1";
			case GrammarType::CFG_2: return "context free-2";
			case GrammarType::RG_3_LEFT_LINEAR: return "regular-3-left-linear";
			case GrammarType::RG_3_RIGHT_LINEAR: return "regular-3-right-linear";
			case GrammarType::RG_3_LINEAR: return "regular-3-linear";
			default: return "invalid";
		}
	}

	Parser(const std::vector<Token> &tokens): mTokens(tokens) {};
	Grammar parseGrammar();

	
	private:
	const std::vector<Token> &mTokens;
	size_t mi = 0; // index of current token

	const Token &peek();
	void skip();
	void badToken();
	void expectToken(Token::TokenType type);
	void expectTokenData(const std::string &data);
	std::vector<Token> parseProdLeftSide();
	std::vector<Token> parseProdRightSide();
};

