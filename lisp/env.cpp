
#include "cell.h"
#include "env.h"

void Environment::set(std::string& name, const Cell& cell) {
		if (exist(name)) {
			_locals[name] = Cell(cell);
		} else {
			throw Uninit(name);
		}
	}

	void Environment::def(std::string& name, const Cell& cell) {
		_locals[name] = Cell(cell);
	}

	Environment::Environment(const std::vector<Cell>& params, const std::vector<Cell>& args, Environment* outer) : _outer(outer) {
	    auto a = args.begin();
	    for (auto p = params.begin(); p != params.end(); ++p) {
	 		_locals[p->value()] = *a++;
	    }
	}

	bool Environment::exist(std::string& name) {
		if (_locals.find(name) != _locals.end())
            return true;
        if (_outer) {
        	return _outer->exist(name);
        }

        return false;
	}

	Cell& Environment::
	get(std::string& name) {
		if (_locals.find(name) != _locals.end())
            return _locals[name];
        if (_outer)
            return _outer->get(name);
        throw Uninit(name);
	}