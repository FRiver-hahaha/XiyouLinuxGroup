#ifndef STORE_H
#define STORE_H

#include <unordered_map>
#include <string>
#include <sstream>
#include <iostream>


using std::cout;    using std::endl;
using std::string;  using hash_map = std::unordered_map<string, string>;


class Store {
    hash_map store;
    string workingDirectory; 

public:
    Store() : workingDirectory(".") {} 
    
    string set(const string& name, const string& age);
    string get(const string& name);
    string del(const string& name);
    string list();
    

    string getWorkingDir() const { return workingDirectory; }
    void setWorkingDir(const string& dir) { workingDirectory = dir; }
};

#endif