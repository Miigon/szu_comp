#pragma once
#include <ctype.h>

inline bool isidentbegin(char c) {
	return isalpha(c) || c == '_';
}

inline bool isident(char c) {
	return isalnum(c) || c == '_';
}

inline bool iswhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline bool issymbol(char c) {
	return !iswhitespace(c) && c != '\'' && c != ';' && c != '|';
}