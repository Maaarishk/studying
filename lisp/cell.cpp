#include "cell.h"

#include "excep.h"
#include "env.h"


bool IsSymbolStr(const std::string& str) {
  if (str[0] <= 'z' && str[0] >= 'a') return true;
  if (str[0] <= 'Z' && str[0] >= 'A') return true;
  return false;
}


Cell Cell::Quote(Environment* env) {
  if (args_.size() != 2) {
    throw Syntax();
  }
  return args_[1];
}


Cell Cell::Qqoute(Environment* env) {
  if (args_.size() != 2) {
    throw Runtime();
  }
  if (args_[1].getType() != EXPR) {
    return args_[1];
  }
  if (args_[1].argumentsCount() == 0) {
    return args_[1];
  }
  if (args_[1][0].value() == ".") {
    throw Syntax();
  }
  if (args_[1][args_[1].argumentsCount() - 1].value() == ".") {
    throw Syntax();
  }
  return args_[1];
}

Cell Cell::Define(Environment* env) {
  if (args_.size() != 3) {
    throw Syntax();
  }
  if (!IsSymbolStr
    (args_[1].value())) {
    throw Syntax();
  }
  env->def(args_[1].value(), args_[2].Evaluate(env));

  return env->get(args_[1].value());
}

Cell Cell::IsNumber(Environment* env) {
  if (args_[1].getType() == NUMBER) return TRUE;
  return FALSE;
}

Cell Cell::Max(Environment* env) {
  Cell max(NUMBER);

  if (args_.size() < 2) {
    throw Runtime();
  }
  max.value() = args_[1].Evaluate(env).value();


  for (size_t i = 1; i < args_.size(); ++i) {
    auto tmp = args_[i].Evaluate(env);

    if (tmp.getType() != NUMBER) {
      throw Runtime();
    }
    if (tmp.value() > max.value()) {
      max = tmp;
    }
  }
  return max;
}

Cell Cell::Min(Environment* env) {
  Cell min(NUMBER);
  if (args_.size() < 2) {
    throw Runtime();
  }
  min.value() = args_[1].Evaluate(env).value();

  for (size_t i = 1; i < args_.size(); ++i) {
    auto tmp = args_[i].Evaluate(env);

    if (tmp.getType() != NUMBER) {
      throw Runtime();
    }
    if (tmp.value() < min.value()) {
      min = tmp;
    }
  }
  return min;
}

Cell Cell::Set(Environment* env) {
  if (args_.size() != 3) {
    throw Syntax();
  }

  env->set(args_[1].value(), args_[2].Evaluate(env));

  return env->get(args_[1].value());
}

Cell Cell::IsSymbol(Environment* env) {
  if (args_.size() != 2) {
    throw Syntax();
  }
  if (args_[1].value()[0] == '\'') {
    args_[1].value().erase(args_[1].value().begin());
  }
  if (args_[1].value()[0] >= 'a' && args_[1].value()[0] <= 'z') {
    return TRUE;
  }
  if (args_[1].value()[0] >= 'A' && args_[1].value()[0] <= 'Z') {
    return TRUE;
  }
  return FALSE;
}

Cell Cell::Abs(Environment* env) {
  if (args_.size() != 2) {
    throw Runtime();
  }

  Cell abs = args_[1].Evaluate(env);
  if (abs.getType() != NUMBER) {
    throw Runtime();
  }
  if (abs.value()[0] == '-') {
    abs.value().erase(abs.value().begin());
  }
  return abs;
}

Cell Cell::Eq(Environment* env) {
  if (args_.size() == 1) {
    return TRUE;
  } 
  if (args_.size() == 2) {
    return TRUE;
  }
  for (size_t i = 2; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    if (args_[i].Evaluate(env).value() != args_[i - 1].Evaluate(env).value()) {
      return FALSE;
    }
  }
  return TRUE;
}

Cell Cell::Less(Environment* env) {
  if (args_.size() == 1) {
    return TRUE;
  } 
  if (args_.size() == 2) {
    return TRUE;
  }
  for (size_t i = 2; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    if (args_[i].Evaluate(env).value() <= args_[i - 1].Evaluate(env).value()) {
      return FALSE;
    }
  }
  return TRUE;
}

Cell Cell::Greater(Environment* env) {
  if (args_.size() == 1) {
    return TRUE;
  } 
  if (args_.size() == 2) {
    return TRUE;
  }
  for (size_t i = 2; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    if (args_[i].Evaluate(env).value() >= args_[i - 1].Evaluate(env).value()) {
      return FALSE;
    }
  }
  return TRUE;
}

Cell Cell::LessEq(Environment* env) {
  if (args_.size() == 1) {
    return TRUE;
  } 
  if (args_.size() == 2) {
    return TRUE;
  }
  for (size_t i = 2; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    if (args_[i].Evaluate(env).value() < args_[i - 1].Evaluate(
      env).value()) {
      return FALSE;
    }
  }
  return TRUE;
}

Cell Cell::GreaterEq(Environment* env) {
  if (args_.size() == 1) {
    return TRUE;
  } 
  if (args_.size() == 2) {
    return TRUE;
  }
  for (size_t i = 2; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    if (args_[i].Evaluate(env).value() > args_[i - 1].
      Evaluate(env).value()) {
      return FALSE;
    }
  }
  return TRUE;
}

Cell Cell::If(Environment* env) {
  if (args_.size() < 3 || args_.size() > 4) {
    throw Syntax();
  }
  if (args_[1].Evaluate(env).value() == "#f") {
    if (args_.size() == 4) {
      return args_[3].Evaluate(env);
    } else {
      return NIL;
      ;
    }
  }
  return args_[2].Evaluate(env);
}

Cell Cell::Plus(Environment* env) {
  if (args_.size() == 1) {
    return Cell(NUMBER, "0");
  }
  int64_t sum = 0;
  for (size_t i = 1; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    sum += std::stoi(args_[i].Evaluate(env).value());
  }
  return Cell(NUMBER, std::to_string(sum));
}

Cell Cell::Mult(Environment* env) {
  if (args_.size() == 1) {
    return Cell(NUMBER, "1");
  }
  int64_t prod = 1;
  for (size_t i = 1; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    prod *= std::stoi(args_[i].Evaluate(env).value());
  }
  return Cell(NUMBER, std::to_string(prod));
}

Cell Cell::Minus(Environment* env) {
  if (args_.size() == 1) {
    throw Runtime();
  }
  int64_t minus = std::stoi(args_[1].Evaluate(env).value());
  for (size_t i = 2; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    minus -= std::stoi(args_[i].Evaluate(env).value());
  }
  return Cell(NUMBER, std::to_string(minus));
}

Cell Cell::Div(Environment* env) {
  if (args_.size() == 1) {
    throw Runtime();
  }
  int64_t divi = std::stoi(args_[1].Evaluate(env).value());
  for (size_t i = 2; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).getType() != NUMBER) {
      throw Runtime();
    }
    divi /= std::stoi(args_[i].Evaluate(env).value());
  }
  return Cell(NUMBER, std::to_string(divi));
}

Cell Cell::IsBoolean(Environment* env) {
  if (args_.size() != 2) {
    throw Runtime();
  }
  if (args_[1].Evaluate(env).getType() == BOOLEAN) {
    return TRUE;
  }
  return FALSE;
}

Cell Cell::And(Environment* env) {
  if (args_.size() == 1) {
    return TRUE;
  }
  for (size_t i = 1; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).value() == "#f") {
      return FALSE;
    }
  }
  return args_[args_.size() - 1].Evaluate(env);
}

Cell Cell::Or(Environment* env) {
  if (args_.size() == 1) {
    return FALSE;
  }
  for (size_t i = 1; i < args_.size(); ++i) {
    if (args_[i].Evaluate(env).value() == "#t") {
      return TRUE;
    }
  }
  return args_[args_.size() - 1].Evaluate(env);
}

Cell Cell::Not(Environment* env) {
  if (args_.size() != 2) {
    throw Runtime();
  }
  return args_[1].Evaluate(env).value() == "#f" ? TRUE : FALSE;
}

Cell Cell::Null(Environment* env) {
  if (args_.size() != 2) {
    throw Runtime();
  }
  return args_[1].Evaluate(env).toString() == "()" ? TRUE : FALSE;
}

Cell Cell::Evaluate(Environment* env) {
  // std::cout << type_ << std::endl;
  if (type_ == NUMBER) {
    return *this;
  }
  if (type_ == BOOLEAN) {
    return *this;
  }
  if (type_ == SYMBOL) {
    if (value_[0] == '\'') {
      value_.erase(value_.begin());
      return *this;
    }
    return env->get(value_)
    ;
  }
  if (type_ == LIST) {
    if (args_[0].value() == "\'") {
      return Qqoute(env);
    }
  }
  if (type_ == EXPR) {
    if (args_.size() == 0) {
      throw Runtime();
    }
    if (args_[0].getType() == SYMBOL) {
      if (args_[0].value() == "#f") {
        return FALSE;
      }
      if (args_[0].value() == "#t") {
        return TRUE;
      }
      if (args_[0].value() == "lambda") { //lambda //
        if (args_.size() < 3) {
          throw Syntax();
        }
        type_ = LAMBDA;
        env_ = env;
        return *this;
      }
      if (args_[0].value() == "quote") {  // quote //
        return Quote(env);
      }
      if (args_[0].value() == "define") {  // define //
        return Define(env);
      }
      if (args_[0].value() == "if") { // if //
        return If(env);
      }
      if (args_[0].value() == "number?") {  // number?
        return IsNumber(env);
      }
      if (args_[0].value() == "max") {  // max
        return Max(env);
      }
      if (args_[0].value() == "min") {  // min
        return Min(env);
      }
      if (args_[0].value() == "set!") {  // set //
        return Set(env);
      }
      if (args_[0].value() == "symbol?") {  // symbol
        return IsSymbol(env);
      }
      if (args_[0].value() == "abs") { // abs
        return Abs(env);
      }
      if (args_[0].value() == "=") { // abs
        return Eq(env);
      }
      if (args_[0].value() == "<") { // abs
        return Less(env);
      }
      if (args_[0].value() == ">") { // abs
        return Greater(env);
      }
      if (args_[0].value() == "<=") { // abs
        return LessEq(env);
      }
      if (args_[0].value() == ">=") { // abs
        return GreaterEq(env);
      }
      if (args_[0].value() == "+") {
        return Plus(env);
      }
      if (args_[0].value() == "*") {
        return Mult(env);
      }
      if (args_[0].value() == "-") {
        return Minus(env);
      }
      if (args_[0].value() == "/") {
        return Div(env);
      }
      if (args_[0].value() == "boolean?") {
        return IsBoolean(env);
      }
      if (args_[0].value() == "and") {
        return And(env);
      }
      if (args_[0].value() == "or") {
        return Or(env);
      }
      if (args_[0].value() == "not") {
        return Not(env);
      }
      if (args_[0].value() == "null?") {
        return Null(env);
      }
    }
  }
  Cell func(args_[0].Evaluate(env));

  std::vector<Cell> args;
  for (size_t i = 1; i < args_.size(); ++i) {
    args.push_back(args_[i].Evaluate(env));
  }
  if (func.getType() == LAMBDA) {

    std::unique_ptr<Environment> inner(new Environment(func[1].args_, args, func.env_));
    for(size_t i = 2; i < func.argumentsCount(); ++i) {
      if (i + 1 == func.argumentsCount()) {
        return func[i].Evaluate(inner.get());
      }
      func[i].Evaluate(inner.get());
    }
  } 
  throw Runtime();
  return TRUE;
}
