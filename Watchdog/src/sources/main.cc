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

#include <atomic>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <csignal>

#include "config.h"

#include "Poco/Process.h"

using std::string;
using std::cout;
using std::atomic;

atomic<int> p_id (0);
atomic<int> ret_code (0);

atomic<bool> processStarted (false);
atomic<bool> processKillRequest (false);
atomic<bool> processRestartRequest (false);

std::thread t;
string program_name = "";
Config config;


void signalHandler (int signum) {
    std::cout << "Watchdog: Interrupt signal (" << signum << ") received."<<std::endl;

    if(processStarted == true && p_id != 0) {
        processKillRequest = true;

        if(signum == 10 || signum == 12) {
            processRestartRequest = true;
        }
        // send signal to process
        if(config.kill_friendly) {
            std::cout<<"friendly killing of process...";
            Poco::Process::requestTermination(p_id);
        } else {
            std::cout<<"hard killing of process...";
            Poco::Process::kill(p_id);
        }
    }
}

void init()  {
    processStarted = false;
    processKillRequest = false;
    processRestartRequest = false;
    p_id = 0;
    ret_code = 0;
}


void startProcess(std::string program, std::vector<std::string> args) { 
    Poco::ProcessHandle ph = Poco::Process::launch(program, args, 0, 0, 0);
    processStarted = true;
    p_id = ph.id();
    ret_code = ph.wait();   
}

void showHelp() {
    cout<<"watchdog <arguments for watchdog> -- [PATH TO EXECUTABLE] <arguments for executable>"<<std::endl;
    cout<<"  -kf OR -kill_friendly=[true,false] | default: true     | kill process friendly with signal or hard kill "<<std::endl;
    cout<<"  -rt OR -restart_tries=[times]      | default: infinite | restart process n times after finish "<<std::endl;
    cout<<"  -cr OR -crash_restart=[true,false] | default: true     | restart a process after it exits abnormally (return code != 0) "<<std::endl;
    cout<<"Use signals to stop or restart the monitored process:"<<std::endl;
    cout<<"  watchdog  -- /usr/bin/susi &"<<std::endl;;
    cout<<"  kill -2 $!  # stops the monitored service"<<std::endl;
    cout<<"  kill -10 $! # restarts the monitored service"<<std::endl;
}

int main(int argc, char** argv){
    
    // register signal handler
    signal(SIGINT,  signalHandler); // Strg-C  -- kill
    signal(SIGUSR1, signalHandler); // 10      -- restart
    signal(SIGUSR2, signalHandler); // 12      -- restart

    std::vector<std::string> watchdog_args;
    std::vector<std::string> filtered_vec;
    std::vector<std::string> programm_args;

    // check help
    if(argc < 2 ) {      
        showHelp();
        exit(1);
    }

    if(argc == 2) {
        std::string o = argv[1];
        if(o == "-h" || o == "-help" || o == "--h" || o == "--help") {
            showHelp();
        }
        exit(0);
    }

    // get arguments
    bool delimeter_found = false; // '--'

    for (int i=1; i<argc; i++) {
        std::string option = argv[i];

        if(option == "--") {
            delimeter_found = true;
            i++;

            if(i < argc) { program_name = argv[i]; i++; }
        }

        if(i < argc) {
            if(delimeter_found) { programm_args.push_back(argv[i]); }
            else{ watchdog_args.push_back(argv[i]); }
        }
        
    }   

    // check delimeter
    if(delimeter_found == false) {
        cout<<"Delimeter \"--\" not found\n";
        exit(0);
    }

    // check programm
    if(program_name == "" || !config.getExecutable(program_name)) {
        cout<<"Process not found or isn't executable\n";
        exit(0);
    }   

    // check watchdog arguments
    filtered_vec = config.parseCommandLine(watchdog_args);

    if(filtered_vec.size() > 0)  {
        cout<<"Some Watchdog commands could not be processed!\n";
        config.printArgs(filtered_vec);
        exit(0);
    }

    cout<<"Watchdog .."<<std::endl;
    cout<<"  PROGRAM: "<<program_name<<"\n";
    cout<<"  KILL_FRIENDLY: "<<((config.kill_friendly == true) ? "true" : "false")<<"\n";
    cout<<"  CRASH_RESTART: "<<((config.restart_crached == true) ? "true" : "false")<<"\n";
    cout<<"  RESTART_TRIES: "<<((config.restart_trys == -1) ? "infinite" : std::to_string(config.restart_trys))<<"\n";

    for(;;) {
        init();

        cout<<"Start Process: "<<program_name<<"\n";

        t = std::thread(startProcess, program_name, programm_args);
        t.join();

        if(ret_code != 0 && config.restart_crached == false) {
            break;
        }

        cout<<"Thread finished with:"<<ret_code<<"\n";
        if(processKillRequest == true) {
            cout<<"processKillRequest after\n";

            if(processRestartRequest == false) exit(0);
        }

        if(config.restart_trys > 0 && config.restart_trys != -1) {
            config.restart_trys--;
        } else {
            if(config.restart_trys == 0) {
                cout<<"Watchdog finished.\n";
                exit(0);
            }
        }
    }       
    

    exit(0);
}