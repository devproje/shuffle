#include "libsapp.h"
#include "commandmgr.h"
#include "console.h"

ENTRYPOINT(Workspace workspace, const vector<string> &args) {
  if (args.size() == 1) {
    vector<string> helps;
    for (const auto &item : commands) {
      Command &command = *item;
      string text = command.getName() + ": " + command.getDescription();
      helps.push_back(text);
    }

    info("Only some commands are shown:");
    info("\033[90m(Type shfl cmds to see all commands.)");
    for (const auto &item : helps) info(item);
  } else if (args.size() == 2) {
    info("system.wip"); // TODO
  } else {
    too_many_arguments();
    return;
  }
}