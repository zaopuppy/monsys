#include "zformatter.h"

#include <stdlib.h>

void ZFormatter::exe(int32_t v)
{
	// printf("ZFormatter::exe()\n");

	arg_.result_.set(v);

	size_t func_count = func_list_.size();
	for (size_t i = 0; i < func_count; ++i) {
		func_list_[i]->exe();
		func_list_[i]->result_.print();
	}
}

bool ZFormatter::compile(const char *script)
{
	if (!script) {
		return false;
	}

	size_t script_len = strlen(script);
	if (script_len <= 0) {
		return false;
	}

	input_ = script;
	input_len_ = script_len;
	input_idx_ = 0;

	ZFunc* func = NULL;
	do {
		func = constStr();
		if (!func) {
			func = varFunc();
		}

		if (func) {
			func_list_.push_back(func);
		}
	} while (func);

	if (input_idx_ != input_len_) {
		printf("failed to compile, current index: ('%c', %u)\n",
			input_[input_idx_], input_idx_);
		return false;
	}

	return true;
}

ZFunc* ZFormatter::constStr()
{
	size_t start_pos = input_idx_;
	size_t end_pos = input_idx_;
	bool has_dollar = false;
	char c;

	while (end_pos < input_len_) {
		c = input_[end_pos];
		if (c == '$') {
			++end_pos;
			has_dollar = true;
		} else if (c == '{' || isIdentifierChar(c)) {
			if (has_dollar) {
				--end_pos;
				break;
			} else {
				++end_pos;
			}
			has_dollar = false;
		} else {
			++end_pos;
		}
	}

	if (end_pos == start_pos) {
		return NULL;
	}

	// update input idx
	input_idx_ += end_pos - start_pos;

	std::string const_str(input_, start_pos, end_pos - start_pos);

	ZStringFunc *func = new ZStringFunc(const_str);
	// printf("const_string: [%s]\n", const_str.c_str());

	return func;
}

ZFunc* ZFormatter::getFuncByName(const std::string &name)
{
	if (name == "div") {
		return new ZDivFunc();
	} else if (name == "multi") {
		return new ZMultiFunc();
	} else {
		printf("unknown function name: [%s]\n", name.c_str());
		return NULL;
	}
}

// ${div(div(1, 2), 4)}
ZFunc* ZFormatter::varFunc()
{
	if (input_[input_idx_] != '$') {
		return NULL;
	}
	++input_idx_;

	///////////////////////////////
	// '{'
	bool has_parenthesis = input_[input_idx_] == '{' ? true : false;
	if (has_parenthesis) ++input_idx_;

	skipBlank();

	///////////////////////////////
	// func_name
	size_t start_pos = input_idx_;
	size_t end_pos = input_idx_;
	char c;
	while (end_pos < input_len_) {
		c = input_[end_pos];
		if (!isIdentifierChar(c)) {
			break;
		}
		++end_pos;
	}

	input_idx_ = end_pos;
	std::string func_name(input_, start_pos, end_pos - start_pos);
	// printf("func_name: [%s]\n", func_name.c_str());

	ZFunc *func;

	if (has_parenthesis) skipBlank();

	///////////////////////////////
	// update input index
	if (input_[input_idx_] == '(') {

		++input_idx_;	// skip '('

		skipBlank();

		func = getFuncByName(func_name);
		if (func == NULL) {
			return NULL;
		}

		// function
		ZFunc *pa;

		do {
			pa = param();
			if (pa == NULL) {
				break;
			}

			func->arg_list_.push_back(pa);

			skipBlank();

			if (input_[input_idx_] != ',') {
				break;
			}

			skipBlank();

			++input_idx_; // skip ','

			skipBlank();
		} while (true);

		if (input_[input_idx_] != ')') {
			return NULL;
		}
		++input_idx_; // skip ')'

	} else {
		// variable, should be 0~9
		if (!isNumber(func_name.c_str())) {
			printf("variable should be a number or function: [%s]\n", func_name.c_str());
			return NULL;
		}

		int arg_num = atoi(func_name.c_str());

		if (arg_num != 1) {
			printf("Only 1 argument is supported\n");
			return NULL;
		}

		return &arg_;
	}

	///////////////////////////////
	// '}'
	if (has_parenthesis) skipBlank();

	if (has_parenthesis && input_[input_idx_] != '}') {
		return NULL;
	}
	if (has_parenthesis) ++input_idx_;

	return func;
}

// param : varFunc | number | '"' string '"'
ZFunc* ZFormatter::param()
{
	char c = input_[input_idx_];

	if (c == '"') {
		// printf("string mitai\n");
		// string
		return quoteStr();

	} else if (isdigit(c)) {
		// printf("number mitai\n");
		// constant number
		return constNumber();

	} else if (c == '$') {
		// printf("variable mitai\n");
		// varFunc
		return varFunc();

	} else {
		printf("WTF\n");
		return NULL;
	}
}

ZFunc* ZFormatter::quoteStr()
{
	// printf("ZFormatter::quoteStr()\n");

	if (input_[input_idx_] != '"') {
		printf("error in quoteStr()\n");
		return NULL;
	}
	// skip '"'
	++input_idx_;

	size_t start_pos = input_idx_;
	size_t end_pos = start_pos;

	while (end_pos < input_len_ && input_[end_pos] != '"') {
		++end_pos;
	}

	if (input_[end_pos] != '"') {
		printf("quote string no ended\n");
		return NULL;
	}

	input_idx_ = end_pos;

	std::string s(input_, start_pos, end_pos - start_pos);

	printf("quote string: [%s]\n", s.c_str());

	ZStringFunc *func = new ZStringFunc(s);

	return func;
}

ZFunc* ZFormatter::constNumber()
{
	// printf("ZFormatter::constNumber()\n");

	size_t start_pos = input_idx_;
	size_t end_pos = start_pos;

	while (end_pos < input_len_ && isdigit(input_[end_pos])) ++end_pos;

	if (end_pos == start_pos) {
		printf("empty in constNumber()\n");
		return NULL;
	}

	input_idx_ = end_pos;

	std::string n(input_, start_pos, end_pos - start_pos);
	// printf("parsed number: [%s]\n", n.c_str());

	ZNumberFunc *func = new ZNumberFunc(atoi(n.c_str()));
	return func;
}

void ZFormatter::skipBlank()
{
	while (input_idx_ < input_len_ && isblank(input_[input_idx_])) ++input_idx_;
}

