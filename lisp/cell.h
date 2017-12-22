#include <iostream>

#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <exception>
#include <memory>



#ifndef LISP_CELL
#define LISP_CELL

#include "excep.h"
#include "env.h"

class Cell {
public:
	enum Type {BOOLEAN, NUMBER, SYMBOL, EXPR, LIST, LAMBDA, FUNC};

	Cell() : env_(nullptr) {};
	Cell(Type type) : env_(nullptr), type_(type) {}

	Cell(Type type, const std::string& value) : type_(type), value_(value) {
		if (type_ == NUMBER) {
			if (value_[0] == '+') {
				value_.erase(value_.begin());
			}
		}
	} 

	Type& getType() { return type_; }

	std::string& value() { return value_; }
	const std::string& value() const { return value_; }
	
	void PushBack(const Cell& arg) {args_.push_back(arg);};

	Cell& operator[](size_t index) { return args_[index]; }
	const Cell& operator[](size_t index) const { return args_[index]; }
	
	size_t argumentsCount() const { return args_.size(); }

	std::string toString() const {
		if (type_ == Type::EXPR) {
			std::string str = "(";

			for (const auto& a: args_) {
				str += a.toString() + " ";
			}

			if (str[str.size() - 1] == ' ') {
				str.erase(str.size() - 1);
			}

			return str + ")";
		} else {
			return value_;
		}
	}
	
	Cell Evaluate(Environment* env);

private:
	Environment* env_;

	std::vector<Cell> args_;
	Type type_;
	std::string value_;

	Cell Quote(Environment* env);
	Cell Define(Environment* env);
	Cell IsNumber(Environment* env);
	Cell Max(Environment* env);
	Cell Min(Environment* env);
	Cell Set(Environment* env);
	Cell IsSymbol(Environment* env);
	Cell Abs(Environment* env);
	Cell Eq(Environment* env);
	Cell Less(Environment* env);

	Cell Greater(Environment* env);
	Cell LessEq(Environment* env);
	Cell GreaterEq(Environment* env);
	Cell If(Environment* env);
	Cell Plus(Environment* env);
	Cell Mult(Environment* env);
	Cell Minus(Environment* env);
	Cell Div(Environment* env);
	Cell IsBoolean(Environment* env);
	Cell And(Environment* env);


	Cell Or(Environment* env);
	Cell Not(Environment* env);
	Cell Null(Environment* env);
	Cell Qqoute(Environment* env);
};

const Cell FALSE(Cell::Type::BOOLEAN, "#f");
const Cell TRUE(Cell::Type::BOOLEAN, "#t");
const Cell NIL(Cell::Type::SYMBOL, "()");


#endif
