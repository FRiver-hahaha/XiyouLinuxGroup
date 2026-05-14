#include "CommandHandler.h"

string CommandHandler::execute(const string command) {// 执行`函数`
    std::istringstream iss(command);
    string cmd;
    iss >> cmd;

    if(cmd.empty()) {
        return "ERROR: 空命令"; 
    }

    string args;
    std::getline(iss, args);
    if(!args.empty() && args[0] ==' ') {
        args = args.substr(1);
    }

    auto it = comTofunc.find(cmd);
    if(it != comTofunc.end()) {
        return it->second(args);
    }

    return "ERROR: 未知命令" + cmd + "输入 HELP 查询可用命令";
}

string CommandHandler::help() {// 获取帮助
    string helpStr = "可使用命令:\n";
    for(const auto& [cmd, _] : comTofunc) {
        auto it = amanHelp.find(cmd);
        if(it != amanHelp.end()) {
            helpStr += "  " + it->second + '\n';
        }else {
            if(cmd != "HELP") {
                helpStr += "  " + cmd + '\n';
            }
        }
    }

    helpStr += "  HELP - 获取可用命令";

    return helpStr;
}

void CommandHandler::registerCmd() {// 建立哈希表
    comTofunc["SET"] = [this](const string& args) -> string {
        std::istringstream iss(args);
        string name, age;
        iss >> name;
        std::getline(iss, age);

        if(!age.empty() && age[0] == ' ') {
            age = age.substr(1);
        }

        if(age.empty() || name.empty()) {
            return "ERROR: Usage: SET <name> <age>";
        }
        return store.set(name, age);
    };

    comTofunc["GET"] = [this](const string& args) -> string {
        std::istringstream iss(args);
        string name;
        iss >> name;

        if(name.empty()) {
            return "ERROR: Usage: GET <name>";
        }
        return store.get(name);
    };

    comTofunc["DEL"] = [this](const string& args) -> string {
        std::istringstream iss(args);
        string name;
        iss >> name;

        if(name.empty()) {
            return "ERROR: Usage: DEL <name>";
        }
        return store.del(name);
    };

    comTofunc["LIST"] = [this](const string&) -> string {return store.list();};
    comTofunc["QUIT"] = [this](const string&) -> string {return "Oops! BYE~";};
    comTofunc["HELP"] = [this](const string&) -> string {return help();};
    amanHelp["SET"] = "SET <name> <age> - 设置名字和年龄";
    amanHelp["GET"] = "GET <name> - 获取名字下的年龄";
    amanHelp["DEL"] = "DEL <name> - 删除名字";
    amanHelp["QUIT"] = "QUIT - 离开服务器";
    amanHelp["LIST"] = "LIST - 列出名字和年龄";
}