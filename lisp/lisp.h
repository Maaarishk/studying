#include "cell.h"

class Lisp {
public:
	Lisp(): globals_(new Environment) {

    }
    ~Lisp() {delete globals_;}

    std::string Exec(std::string expression) {
    	if (Balance(expression) != 0) {throw Syntax();}
    	
    	auto tokens = Tokenize(expression);
      	return Parse(tokens).Evaluate(globals_).toString();
    }

private:
int Balance(const std::string& str) {
  int balance = 0;
  for (size_t i = 0; i < str.size(); ++i) {
  	switch(str[i]) {
  		case '(': {
  			++balance;
  			break;
  		}
  		case ')': {
  			--balance;
  			break;
  		}
  	}

    if (balance < 0) {
      return balance;
    }
  }
  return balance;
}

Cell Parse(std::list<std::string>& tokens) {
  std::string token = tokens.front();
  tokens.pop_front();
  if (token == "(") {
    Cell expr(Cell::Type::EXPR);
    while (tokens.size() && tokens.front() != ")") {
      expr.PushBack(Parse(tokens));
    }
    if (tokens.size() == 0) {
      throw Syntax();
    }
    tokens.pop_front();
    return expr;
  } else if (token == ")") {
    throw Syntax();
  } else if (token == "\'") {
    Cell expr(Cell::Type::LIST);
    expr.PushBack(Cell(Cell::Type::SYMBOL, "\'"));
    if (tokens.size()) {
      expr.PushBack(Parse(tokens));
    }
    return expr;
  } else {
    if (token == "#t") {
      return TRUE;
    } else if (token == "#f") {
      return FALSE;
    } else if (IsNumber(token)) {
      return Cell(Cell::Type::NUMBER, token);
    } else {
      return Cell(Cell::Type::SYMBOL, token);
    }
  }
}


bool IsNumber(const std::string& value) {
  size_t start = 0;
  if (value[0] == '+' || value[0] == '-') {
    if (value.size() == 1) {
      return false;
    }
    ++start;
  }
  for (size_t i = start; i < value.size(); ++i) {
    if (value[i] < '0' || value[i] > '9') {
      return false;
    }
  }
  return true;
}


bool IsSymbol(const std::string& str) {
  if (str[0] <= 'z' && str[0] >= 'a') return true;
  if (str[0] <= 'Z' && str[0] >= 'A') return true;
  return false;
}

std::list<std::string> Tokenize(const std::string& scheme_line) {
  std::list<std::string> tokens;
  const char* cur = scheme_line.c_str();
  while (*cur) {
    while (*cur == ' ' || *cur == '\n') {
      ++cur;
    }
    if (*cur == '(') {
      tokens.push_back("(");
      ++cur;
    } else if (*cur == ')') {
      tokens.push_back(")");
      ++cur;
    } else {
      const char* next = cur;
      while (*next && *next != ' ' && *next != '(' && *next != ')') ++next;
      tokens.push_back(std::string(cur, next));
      cur = next;
    }
  }
  return tokens;
}

	Environment* globals_;
};


