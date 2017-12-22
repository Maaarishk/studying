#include <iostream>

#include <string>
#include <list>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <exception>
#include <memory>


// excep.h

#include <exception>

#ifndef LISP_EXCEPTION
#define LISP_EXCEPTION

class Syntax : public std::exception {
public:
	virtual const char* what() const noexcept{
		return "Syntax Error";
	}
};

class Uninit : public std::exception {
public:
	Uninit(std::string name) : _name(name) {

	}
    virtual const char* what() const noexcept {
		return (std::string("Uninitiolaze Variable: ") + _name).c_str();
	}
private:
	std::string _name;
};

class Runtime : public std::exception {
public:
	virtual const char* what() const noexcept{
		return "Runtime Error";
	}
};

#endif
