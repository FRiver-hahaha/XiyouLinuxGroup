#include "Store.h"

string Store::set(const string& name, const string& age) {
    store[name] = age;
    return "OK: SET " + name + "<=>" + age;
}

string Store::get(const string& name) {
    auto it = store.find(name);
    if(it != store.end()) {
        return "OK: GET " + it->second;
    }
    return "ERROR: not found";
}

string Store::del(const string& name) {
    auto it = store.find(name);
    if(it != store.end()) {
        store.erase(name);
        return "OK: DEL " + name;
    }
    return "ERROR: not found";
}

string Store::list() {
    if(store.empty()) {
        return "ERROR: store is empty";
    }
    std::stringstream ss;
    ss << "LIST: ";
    for(auto& pair : store) {
        ss << " " << pair.first << "<=>" << pair.second << '\n';
    }
    return ss.str();
}