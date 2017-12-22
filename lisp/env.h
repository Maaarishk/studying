// environment.h

#ifndef LISP_ENVIRONMENT
#define LISP_ENVIRONMENT

class Cell;

typedef std::map<std::string, Cell> Vars;

class Environment {
public:
	Environment(): _outer(nullptr) {
		
	}

	Environment(Environment* outer) : _outer(outer){

	}

	Environment(const std::vector<Cell>& params, const std::vector<Cell>& args, Environment* outer);

	bool exist(std::string& name);

	Cell& get(std::string& name);

	void set(std::string& name, const Cell& cell);

	void def(std::string& name, const Cell& cell);

private:
	Vars _locals;
	Environment* _outer;
};

static Environment* GLOBALS = new Environment();

#endif