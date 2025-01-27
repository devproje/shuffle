#include "commandmgr.h"

#include <iostream>
#include <json/json.h>
#include <memory>
#include <utility>

#include "console.h"
#include "suggestion.h"
#include "utils/utils.h"
#include "basic_commands.h"
#include "sapp/sapp.h"

#define COMMANDS_JSON (DOT_SHUFFLE + "/commands.json")

using namespace std;

vector<unique_ptr<Command>> commands;

void loadDefaultCommands() {
  commands.push_back(make_unique<Command>(Command(
      "shfl", "Shuffle Command",
      {
          Command("reload", "Reload all commands"),
          Command("apps", "Manage SAPP",
                  {Command("add", "Install SAPP"),
                   Command("remove", "Delete SAPP")}),
      })));

  commands.push_back(make_unique<Command>(Command("list", "Print list file of current directory", listCmd)));
  commands.push_back(make_unique<Command>(Command(
      "lang", "Change Language", langCmd,
      {Command("en_us"),
       Command("ko_kr")})));
  commands.push_back(make_unique<Command>(Command("exit", "Shut down Shuffle", exitCmd)));
  commands.push_back(make_unique<Command>(Command("clear", "Clear console", clearCmd)));
}

void loadCommand(const CommandData &data) {
  commands.push_back(make_unique<SAPPCommand>(SAPPCommand(data.name)));
}

vector<CommandData> getRegisteredCommands() {
  vector<CommandData> res;

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(COMMANDS_JSON), root, false);

  Json::Value commandList = root["commands"];
  for (auto command : commandList) {
    CommandData data;
    data.name = command["name"].asString();
    res.push_back(data);
  }

  return res;
}

void addRegisteredCommand(const CommandData &data) {
  vector<CommandData> res;

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(COMMANDS_JSON), root, false);

  Json::Value commandList = root["commands"];

  Json::Value value(Json::objectValue);
  value["name"] = data.name;
  value["value"] = data.value;
  commandList.append(value);

  root["commands"] = commandList;

  writeFile(COMMANDS_JSON, root.toStyledString());
}

void loadCommands() {
  commands.clear();

  loadDefaultCommands();

  for (const CommandData &command : getRegisteredCommands()) {
    loadCommand(command);
  }
}

Command findCommand(const string &name, const vector<unique_ptr<Command>>& DICTIONARY) {
  for (const auto &item : DICTIONARY) {
    Command &command = *item;
    if (command.getName() == name) return command;
  }
  return Command("");
}

Command findCommand(const string &name) {
  return findCommand(name, commands);
}

const string &Command::getName() const {
  return name;
}

const string &Command::getDescription() const {
  return description;
}

const string &Command::getValue() const {
  return value;
}

vector<unique_ptr<Command>> Command::getChildren() const {
  vector<unique_ptr<Command>> newChildren;
  newChildren.reserve(children.size());
  for (const Command &item : children) {
    newChildren.push_back(make_unique<Command>(item));
  }
  return newChildren;
}

void Command::run(Workspace &ws, const vector<std::string> &args) const {
  executor(ws, args);
}

Command::Command(string name, string description, CommandExecutor executor, vector<Command> children)
    : name(std::move(name)),
      description(std::move(description)),
      executor(executor),
      children(std::move(children)) {}

Command::Command(string name, string description, CommandExecutor executor)
    : name(std::move(name)),
      description(std::move(description)),
      executor(executor) {}

Command::Command(string name, string description, vector<Command> children)
    : name(std::move(name)),
      description(std::move(description)),
      children(std::move(children)) {}

Command::Command(string name, string description)
    : name(std::move(name)),
      description(std::move(description)) {}

Command::Command(string name, vector<Command> children)
    : name(std::move(name)),
      description("-"),
      children(std::move(children)) {}

Command::Command(string name)
    : name(std::move(name)),
      description("-") {}
