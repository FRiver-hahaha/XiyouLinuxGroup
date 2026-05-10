#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace fs = std::filesystem;

class FileHandler {
private:
    fs::path rootDir;
    fs::path originalRoot;
    
    static std::time_t to_time_t(const fs::file_time_type& ftime) {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    }
    
    fs::path resolvePath(const fs::path& p) const {
        try {
            return fs::canonical(fs::absolute(p));
        } catch (...) {
            return fs::absolute(p);
        }
    }
    
public:
    FileHandler(const std::string& root = ".") {
        try {
            rootDir = fs::canonical(fs::absolute(root));
        } catch (...) {
            fs::create_directories(root);
            rootDir = fs::canonical(fs::absolute(root));
        }
        originalRoot = rootDir;
        
        if (!fs::exists(rootDir)) {
            fs::create_directories(rootDir);
        }
    }
    
    std::string listDirectory(const std::string& path = "") {
        std::stringstream ss;
        fs::path fullPath;
        
        if (path.empty() || path == "." || path == "/") {
            fullPath = rootDir;
        } else {
            std::string cleanPath = path;
            if (!cleanPath.empty() && cleanPath[0] == '/') {
                cleanPath = cleanPath.substr(1);
            }
            fullPath = rootDir / cleanPath;
        }
        
        try {
            fullPath = resolvePath(fullPath);
            std::string fullPathStr = fullPath.string();
            std::string rootStr = originalRoot.string();
            if (fullPathStr.find(rootStr) != 0) {
                return "550 Permission denied\r\n";
            }
        } catch (...) {
            return "550 Directory not found\r\n";
        }
        
        if (!fs::exists(fullPath) || !fs::is_directory(fullPath)) {
            return "550 Directory not found\r\n";
        }
        
        try {
            for (const auto& entry : fs::directory_iterator(fullPath)) {
                auto ftime = fs::last_write_time(entry);
                auto cftime = to_time_t(ftime);
                
                if (fs::is_directory(entry)) {
                    ss << "drwxr-xr-x 1 owner group ";
                } else {
                    ss << "-rw-r--r-- 1 owner group ";
                }
                
                auto fileSize = fs::file_size(entry);
                ss << std::setw(10) << fileSize << " ";
                ss << std::put_time(std::localtime(&cftime), "%b %d %H:%M ");
                ss << entry.path().filename().string() << "\r\n";
            }
        } catch (const std::exception& e) {
            return "550 Error listing directory: " + std::string(e.what()) + "\r\n";
        }
        
        return ss.str();
    }
    
    bool getFilePath(const std::string& filename, fs::path& fullPath) {
        if (filename.empty()) return false;
        
        std::string cleanName = filename;
        if (!cleanName.empty() && cleanName[0] == '/') {
            cleanName = cleanName.substr(1);
        }
        
        fullPath = rootDir / cleanName;
        
        try {
            fullPath = resolvePath(fullPath);
            std::string fullPathStr = fullPath.string();
            std::string rootStr = originalRoot.string();
            if (fullPathStr.find(rootStr) != 0) {
                return false;
            }
        } catch (...) {
            return false;
        }
        
        return fs::exists(fullPath) && fs::is_regular_file(fullPath);
    }
    
    std::ifstream getFileForReading(const std::string& filename) {
        fs::path fullPath;
        if (!getFilePath(filename, fullPath)) {
            return std::ifstream();
        }
        return std::ifstream(fullPath, std::ios::binary);
    }
    
    std::ofstream getFileForWriting(const std::string& filename) {
        if (filename.empty()) {
            return std::ofstream();
        }
        
        std::string cleanName = filename;
        if (!cleanName.empty() && cleanName[0] == '/') {
            cleanName = cleanName.substr(1);
        }
        
        fs::path fullPath = rootDir / cleanName;
        
        try {
            fullPath = resolvePath(fullPath);
            std::string fullPathStr = fullPath.string();
            std::string rootStr = originalRoot.string();
            if (fullPathStr.find(rootStr) != 0) {
                return std::ofstream();
            }
        } catch (...) {
            return std::ofstream();
        }
        
        fs::create_directories(fullPath.parent_path());
        
        return std::ofstream(fullPath, std::ios::binary);
    }
    
    bool changeDirectory(const std::string& path) {
        fs::path newPath;
        
        if (path.empty() || path == ".") {
            return true;
        } else if (path == "..") {
            newPath = rootDir.parent_path();
        } else if (path == "/") {
            newPath = originalRoot;
        } else {
            std::string cleanPath = path;
            if (!cleanPath.empty() && cleanPath[0] == '/') {
                cleanPath = cleanPath.substr(1);
            }
            newPath = rootDir / cleanPath;
        }
        
        try {
            newPath = resolvePath(newPath);
            std::string newPathStr = newPath.string();
            std::string rootStr = originalRoot.string();
            if (newPathStr.find(rootStr) != 0) {
                return false;
            }
        } catch (...) {
            return false;
        }
        
        if (fs::exists(newPath) && fs::is_directory(newPath)) {
            rootDir = newPath;
            return true;
        }
        return false;
    }
    
    std::string currentDirectory() const {
        try {
            std::string rootStr = originalRoot.string();
            std::string currentStr = rootDir.string();
            
            if (currentStr == rootStr) {
                return "/";
            }
            
            if (currentStr.find(rootStr) == 0) {
                std::string relative = currentStr.substr(rootStr.length());
                if (relative.empty()) {
                    return "/";
                }
                if (relative[0] != '/') {
                    relative = "/" + relative;
                }
                return relative;
            }
            
            return "/";
        } catch (...) {
            return "/";
        }
    }
};

#endif