#include "susi/ShellComponent.h"
#include "process.hpp"

Susi::ShellComponent::ShellComponent(Susi::SusiClient &susi,
                                     BSON::Value &config)
    : _susi{susi}, _commands{config["commands"].getObject()} {
  _susi.registerProcessor("shell::exec", [this](Susi::EventPtr event) {
    handleExecEvent(std::move(event));
  });
}

void Susi::ShellComponent::join() { _susi.join(); }

void Susi::ShellComponent::handleExecEvent(Susi::EventPtr event) {
  if (!event->payload.isObject() || !event->payload["command"].isString()) {
    throw std::runtime_error{"ShellComponent: need 'command' in payload"};
  }
  std::string command = event->payload["command"].getString();
  if (_commands.count(command) == 0) {
    throw std::runtime_error{
        "ShellComponent: no valid command, check your config file"};
  }
  command = _commands[command].getString();
  if (event->payload["args"].isObject()) {
    BSON::Object args = event->payload["args"].getObject();
    for (auto &kv : args) {
      size_t pos = std::string::npos;
      if ((pos = command.find("$" + kv.first)) != std::string::npos) {
        if (!kv.second.isString()) {
          throw std::runtime_error{
              "ShellComponent: all arguments need to be strings"};
        }
        std::string replacement = kv.second.getString();
        command.replace(pos, kv.first.size() + 1, replacement);
      }
    }
  }
  std::string stdin{""}, stdout{""}, stderr{""};
  if (event->payload["stdin"].isString()) {
    stdin = event->payload["stdin"].getString();
  }
  int status;
  handleExec(command, stdin, stdout, stderr, status);
  event->payload["stdout"] = stdout;
  event->payload["stderr"] = stderr;
  event->payload["status"] = status;
}

void Susi::ShellComponent::handleExec(std::string command, std::string &stdin,
                                      std::string &stdout, std::string &stderr,
                                      int &status) {
  std::cout << "executing command: " << command << std::endl;
  Process proc("bash", "",
               [&stdout](const char *bytes, size_t n) {
                 stdout += std::string(bytes, n);
               },
               [&stderr](const char *bytes, size_t n) {
                 stderr += std::string(bytes, n);
               },
               true);
  proc.write(command + "\n");
  proc.write(stdin);
  proc.close_stdin();
  status = proc.get_exit_status();
}
