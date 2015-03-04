/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#include "gtest/gtest.h"
#include "susi/util/ConstrainedScheduler.h"

#include <ctime>
#include <algorithm>
#include <cstdlib>

bool checkConstraint(std::vector<std::string> plan, std::pair<std::string,std::string> constraint){
	for(size_t i=0;i<plan.size();i++){
		std::string task = plan[i];
		if(task==constraint.first){
			for(size_t j=i+1;j<plan.size();j++){
				if(plan[j]==constraint.second){
					return true;
				}
			}
			return false;
		}
	}
	return false;
}

TEST(ConstrainedScheduler,Basic){
	Susi::Util::ConstrainedScheduler scheduler;

	std::set<std::string> tasks{"a","b","c"};
	scheduler.addConstraint({"c","b"});
	auto res = scheduler.schedule(tasks);

	EXPECT_TRUE( checkConstraint(res, {"c","b"}) );
}

TEST(ConstrainedScheduler,Circle){
	Susi::Util::ConstrainedScheduler scheduler;

	std::set<std::string> tasks{"a","b","c"};
	scheduler.addConstraint({"a","b"});
	scheduler.addConstraint({"b","c"});
	scheduler.addConstraint({"c","a"});
	
	EXPECT_THROW({
		scheduler.schedule(tasks);
	},std::runtime_error);	
}

TEST(ConstrainedScheduler,BigTaskList){
	using Susi::Util::ConstrainedScheduler;
	std::srand ( std::time(nullptr) );
	ConstrainedScheduler scheduler;
	
	//create tasks
	int taskCount = 1000;
	std::vector<std::string> tasks;
	for(int i=0;i<taskCount;i++){
		std::string taskID = std::to_string(i);
		tasks.push_back(std::to_string(i));
	}
	
	//create constraints for 20% of the tasks
	int constraintCount = taskCount/5;
	std::vector<std::pair<std::string,std::string>> constraints;
	for(int i=0;i<constraintCount;i++){
		int idx1 = std::rand()%taskCount;
		int idx2 = idx1+std::rand()%(taskCount-idx1); //this ensures that no circles are created
		if(idx1<idx2)constraints.push_back({tasks[idx1],tasks[idx2]});
	}
	
	//make it a bit difficult
	std::random_shuffle(tasks.begin(),tasks.end());
	std::random_shuffle(constraints.begin(),constraints.end());

	for(auto & c : constraints){
		scheduler.addConstraint(c);
	}
	auto plan = scheduler.schedule(std::set<std::string>{tasks.begin(),tasks.end()});
	for(auto & c : constraints){
		EXPECT_TRUE( checkConstraint(plan,c) );
	}
}
