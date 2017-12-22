#include <iostream>

#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <exception>
#include <memory>

#include "lisp.h"



int balance(const std::string& str) {
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

void REPL(std::istream& istream) {

  Lisp lisp;

  for (;;) {
    try {
      std::string scheme_line;
      std::string tmp;
      while (1) {
        if (istream >> tmp) {
          scheme_line += " ";
          scheme_line += tmp;
          if (balance(scheme_line) == 0) {
            break;
          }
          if (balance(scheme_line) < 0) {
            throw Syntax();
          }
        } else {
          if (balance(scheme_line) > 0) {
            throw Syntax();
          } 
          return;
        }
      }

      std::cout << lisp.Exec(scheme_line) << std::endl;
    } catch (const Runtime& err) {
      std::cout << err.what() << std::endl;
    } catch (const Syntax& err) {
      std::cout << err.what() << std::endl;
      exit(0);
    } catch (const Uninit& err) {
      std::cout << err.what() << std::endl;
    }
  }
}


int main() {
  REPL(std::cin);
  return 0;
}
