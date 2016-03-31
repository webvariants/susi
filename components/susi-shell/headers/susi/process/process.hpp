/**
 * An object to fork a process with a new command. Behaves somewhat like
 * system() except that it doesn't process the command through a shell,
 * so shell syntax won't work. The object provides streams for the child
 * process stdin, stdout and stderr, which you can connect to and read
 * or write.
 *
 * Copyright 2014 Bruce Ide
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _HPP_PROCESS
#define _HPP_PROCESS

#include <boost/algorithm/string.hpp>
#include "susi/process/fdes_stream.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace fr {

    class process {

      std::vector<char *> parameters;
      int in_pipes[2];
      int out_pipes[2];
      int err_pipes[2];

      // Check pipes to see if they're compatible with
      // tubes
      void check_pipe(int status)
      {
	if (status == -1) {
	  throw std::logic_error("Pipe error");
	}
      }

      public:

      pid_t child;
      fr::socket::fdes_stream *in;
      fr::socket::fdes_stream *out;
      fr::socket::fdes_stream *err;

      /**
       * Create the process with the command you want to run
       * (ie: "ls -al blarg") It will break up the command
       * into parameters and store them. The child process
       * won't initiate until start is called.
       */

      process(std::string command) : child(-1), in(nullptr), out(nullptr), err(nullptr)
      {
        std::vector<std::string> parameter_vector;
        parameter_vector.push_back("/bin/bash");
        parameter_vector.push_back("-c");
        parameter_vector.push_back(command);
        /*
        std::string currentParameter = "";

        for(size_t i=0;i<command.size();i++){
            char c = command[i];
            if(i==command.size()-1){
                currentParameter += c;
                parameter_vector.push_back(currentParameter);
            }else if(c==' ' || c=='\t'){
                parameter_vector.push_back(currentParameter);
                currentParameter = "";
            }else if(c == '\"' && i!=0 && command[i-1]!='\\'){
                while(command[++i]!='\"' && i!=0 && command[i-1]!='\\' && i<command.size()){
                    currentParameter += command[i];
                }
                parameter_vector.push_back(currentParameter);
                currentParameter = "";
            }else if(c == '\'' && i!=0 && command[i-1]!='\\'){
                while(command[++i]!='\'' && i!=0 && command[i-1]!='\\' && i<command.size()){
                    currentParameter += command[i];
                }
                parameter_vector.push_back(currentParameter);
                currentParameter = "";
            }else{
                currentParameter += c;
            }
        }
        */
        for(int i = 0 ; i < parameter_vector.size(); ++i) {
          parameters.push_back(new char[parameter_vector[i].length() + 1]);
          strncpy(parameters[i], parameter_vector[i].c_str(), parameter_vector[i].length() + 1);
        }
      }

      ~process()
      {
	if (parameters.size() > 0) {
	  char **p = &parameters[0];
	  while (**p++) {
	    delete *p;
	  }
	}

	if (in) {
	  delete in;
	}
	if (out) {
	  delete out;
	}
	if (err) {
	  delete err;
	}
      }

      // Start the child process.
      pid_t start()
      {
	// Create and check pipes.
	// Will throw logic error if your pipe has an error.
	// I've never seen this happen, but it could.
	check_pipe(pipe(in_pipes));
	check_pipe(pipe(out_pipes));
	check_pipe(pipe(err_pipes));

	child = fork();
	if (child == 0) {
	  // we're the child
	  // Close std file descriptors
	  close(STDIN_FILENO);
	  close(STDOUT_FILENO);
	  close(STDERR_FILENO);
	  // Close pipe endpoints we're not using
	  // Child is READING parent stdin and
	  // WRITING parent stdout/stderr
	  close(in_pipes[1]);
	  close(out_pipes[0]);
	  close(err_pipes[0]);
	  // Attach pipes to file descriptors
	  dup2(in_pipes[0], STDIN_FILENO);
	  dup2(out_pipes[1], STDOUT_FILENO);
	  dup2(err_pipes[1], STDERR_FILENO);

	  parameters.push_back((char *) NULL);
	  // Launch new program
	  execvp(parameters[0], &parameters[0]);
	}

	close(in_pipes[0]);
	close(out_pipes[1]);
	close(err_pipes[1]);

	in = new fr::socket::fdes_stream(in_pipes[1]);
	out = new fr::socket::fdes_stream(out_pipes[0]);
	err = new fr::socket::fdes_stream(err_pipes[0]);

	return child;
      }

      // Convenience function. Returns child process
      // status (see man waitpid for how to read it)
      // Or you could write your own much more
      // complex one...

      int join()
      {
	int status;
	int options = 0;
	waitpid(child, &status, options);
	return status;
      }

    };
}

#endif
