#ifndef Z_FORMATER_H__
#define Z_FORMATER_H__

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <assert.h>

#include "stdint.h"

class ZResult {
 public:
 	enum Type {
 		INVALID,
 		// SIGN_8,
 		// UNSIGN_8,
 		// SIGN_16,
 		// UNSIGN_16,
 		SIGN_32,
 		// UNSIGN_32,
 		FLOAT,
 		STRING,
 	};

 public:
 	ZResult(): type_(INVALID) {
 		memset(&value_, 0x00, sizeof(value_));
 	}

 public:
 	bool toFloat(float &flot) {
 		switch (type_) {
 			case SIGN_32:
 			{
 				flot = (float)value_.s32;
 				return true;
 			}
 			case FLOAT:
 			{
 				flot = value_.flot;
 				return true;
 			}
 			default:
 			{
 				return false;
 			}
 		}
 	}

 	void set(int32_t v) {
 		type_ = SIGN_32;
 		value_.s32 = v;
 	}

 	void print() {
 		switch (type_) {
 			case SIGN_32:
 			{
 				printf("s32: [%d]\n", value_.s32);
 				break;
 			}
 			case FLOAT:
 			{
 				printf("float: [%f]\n", value_.flot);
 				break;
 			}
 			case STRING:
 			{
 				printf("string: [%s]\n", value_.str);
 				break;
 			}
 			default:
 			{
 				printf("not initialized\n");
 				break;
 			}
 		}
 	}

 public:
 	int type_;
 	union {
		int8_t 		s8;
		uint8_t 	u8;
		int16_t 	s16;
		uint16_t 	u16;
		int32_t 	s32;
		uint32_t	u32;
		float			flot;
		const char *str;
 	} value_;
};

class ZFunc {
 public:
 	virtual int exe() = 0;
 	virtual int format(char *buf, uint32_t buf_len) = 0;

 public:
 	std::vector<ZFunc*> arg_list_;
 	ZResult result_;
};

class ZStringFunc : public ZFunc {
 public:
 	ZStringFunc(const std::string str): str_(str) {
 		result_.type_ = ZResult::STRING;
 		result_.value_.str = str_.c_str();
 	}
 	ZStringFunc(const char *str, uint32_t str_len): str_(str, str_len) {
 		result_.type_ = ZResult::STRING;
 		result_.value_.str = str_.c_str();
 	}

 	virtual int exe() {
 		// printf("ZStringFunc::exe()\n");
 		// printf("Result: [%s]\n", str_.c_str());
 		return 0;
 	}

 	virtual int format(char *buf, uint32_t buf_len) {
 		return 0;
 	}

 public:
 	std::string str_;
};

class ZDivFunc : public ZFunc {
 public:
 	virtual int exe() {
 		// printf("ZDivFunc::exe()\n");

 		assert(arg_list_.size() == 2);

 		arg_list_[0]->exe();
 		arg_list_[1]->exe();

 		ZResult pa1 = arg_list_[0]->result_;
 		ZResult pa2 = arg_list_[1]->result_;

 		float divider, dividend;

 		// pa1 - dividend
 		if (!pa1.toFloat(dividend)) return -1;
 		// pa2 - divider
 		if (!pa2.toFloat(divider)) return -1;

 		result_.value_.flot = dividend / divider;
 		result_.type_ = ZResult::FLOAT;

 		return 0;
 	}
 	virtual int format(char *buf, uint32_t buf_len) {
 		return 0;
 	}

 public:
};

class ZMultiFunc : public ZFunc {
 public:
 	virtual int exe() {
 		// printf("ZMultiFunc::exe()\n");
 		return 0;
 	}
 	virtual int format(char *buf, uint32_t buf_len) {
 		return 0;
 	}
};

class ZNumberFunc : public ZFunc {
 public:
 	ZNumberFunc() {}
 	ZNumberFunc(int32_t v) {
 		result_.type_ = ZResult::SIGN_32;
 		result_.value_.s32 = v;
 	}

 	virtual int exe() {
 		// printf("ZNumberFunc::exe()\n");
 		return 0;
 	}
 	virtual int format(char *buf, uint32_t buf_len) {
 		return 0;
 	}

 public:
};

class ZParamFunc : public ZFunc {
 public:
 	virtual int exe() {
 		// printf("ZParamFunc::exe()\n");
 		return 0;
 	}
 	virtual int format(char *buf, uint32_t buf_len) {
 		return 0;
 	}

 public:
};

// expr: (const_string|func)+
// const_string: '"' [^$]* '"' | e
// func: '$' '{'? var '(' ')' '}'?

class ZFormatter {
 public:
 	ZFormatter() {}

 public:
 	bool compile(const char *script);
 	void exe(int32_t v);

 private:
 	ZFunc* constStr();
 	ZFunc* varFunc();
 	ZFunc* param();
 	ZFunc* quoteStr();
 	ZFunc* constNumber();

 	ZFunc* getFuncByName(const std::string &name);

 	void skipBlank();
 	bool isIdentifierChar(char c) { return c == '_' || isdigit(c) || isalpha(c); }
 	bool isNumber(const char *str) {
 		while (*str) {
 			if (!isdigit(*str)) {
 				return false;
 			}
 			++str;
 		}

 		return true;
 	}

 private:
 	const char *input_;
 	uint32_t input_len_;	// const
 	uint32_t input_idx_;
 	std::vector<ZFunc*> func_list_;
 	// std::vector<ZFunc*> arg_list_;
 	ZNumberFunc arg_;
};

#endif

