#include "CommandHandler.h"
#include <algorithm>  


string CommandHandler::execute(const string command) {
    std::istringstream iss(command);
    string cmd;
    iss >> cmd;

    if(cmd.empty()) {
        return "500 空命令\r\n"; 
    }

    string args;
    std::getline(iss, args);
    if(!args.empty() && args[0] ==' ') {
        args = args.substr(1);
    }

    // 转换为大写
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

    auto it = comTofunc.find(cmd);
    if(it != comTofunc.end()) {
        return it->second(args);
    }

    return "502 命令未实现：" + cmd + "\r\n";
}

string CommandHandler::help() {
    string helpStr = "FTP 支持命令:\r\n";
    helpStr += "  PASV - 进入被动模式\r\n";
    helpStr += "  LIST - 列出目录内容\r\n";
    helpStr += "  RETR <filename> - 下载文件\r\n";
    helpStr += "  STOR <filename> - 上传文件\r\n";
    helpStr += "  PWD - 显示当前目录\r\n";
    helpStr += "  QUIT - 退出连接\r\n";
    return helpStr;
}

void CommandHandler::registerCmd() {
    comTofunc["LIST"] = [this](const string&) -> string {
        return "准备发送目录列表\r\n226 传输完成\r\n";
    };
    
    comTofunc["QUIT"] = [this](const string&) -> string {
        return "再见\r\n";
    };
    
    comTofunc["HELP"] = [this](const string&) -> string {
        return help();
    };
    
    comTofunc["USER"] = [this](const string&) -> string {
        return "登录成功\r\n";
    };
    
    comTofunc["PASS"] = [this](const string&) -> string {
        return "登录成功\r\n";
    };
}