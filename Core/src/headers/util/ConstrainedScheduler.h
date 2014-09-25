/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __CONSTRAINEDSCHEDULER__
#define __CONSTRAINEDSCHEDULER__

#include <set>
#include <vector>
#include <string>

namespace Susi {
namespace Util {

class ConstrainedScheduler {
protected:
	typedef std::pair<std::string,std::string> Constraint;
	std::set<Constraint> _constraints;

public:	
	void addConstraint(std::string left, std::string right){
		_constraints.insert(Constraint{left,right});
	}

	std::vector<std::string> schedule(std::set<std::string> tasks){
		auto constraints = getNeededConstraints(tasks);
		std::vector<std::string> result;
		bool hasChange = true;
		while(!tasks.empty() && hasChange){
			hasChange = false;
			for(auto & task : tasks){
				if(countAncestors(constraints,task)==0){
					removeConstraints(constraints,task);
					result.push_back(task);
					tasks.erase(task);
					hasChange = true;
				}
			}
		}
		if(!hasChange){
			throw std::runtime_error{"you requested something impossible"};
		}
		return result;
	}

private:
	std::set<Constraint> getNeededConstraints(const std::set<std::string> & tasks){
		std::set<Constraint> result;
		for(auto & constraint : _constraints){
			for(auto & task : tasks){
				if(constraint.first == task || constraint.second == task){
					result.insert(constraint);
					break;
				}
			}
		}
		return result;
	}

	void removeConstraints(std::set<Constraint> & constraints, std::string leftHandSide){
		for(auto & c : constraints){
			if(c.first == leftHandSide){
				constraints.erase(c);
			}
		}
	}

	size_t countAncestors(const std::set<Constraint> & constraints, std::string value){
		size_t count = 0;
		for(auto & c : constraints){
			if(c.second == value){
				count++;
			}
		}
		return count;
	}
};

}
}

#endif // __CONSTRAINEDSCHEDULER__
