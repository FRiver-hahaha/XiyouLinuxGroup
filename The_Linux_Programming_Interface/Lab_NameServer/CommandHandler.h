#ifndef CMDHANDLER_H
#define CMDHANDLER_H

#include <string>
#include <unordered_map>
#include <functional>
#include "Store.h"


/*
    用来解析命令行
    给每个命令绑定函数，输入对应的命令执行对应函数
    使用哈希表，效率高
    处理了参数不存在，参数错误等人为错误性问题
*/

// using std::cout;    using std::endl;
// using std::string;  using hash_map = std::unordered_map<string, string>;


class CommandHandler {
    Store& store;

    using cmdFunc = std::function<string (const string&)>;

    std::unordered_map<string, cmdFunc> comTofunc;
    hash_map amanHelp;

public:
    CommandHandler(Store& storeref) : store(storeref) {// 初始化
        registerCmd();
    }

    string execute(const string command);// 执行command下的函数
    string help();// 查询帮助

private:
    void registerCmd();// 注册哈希表<string> <functional<string (const string&)>>
};

#endif
