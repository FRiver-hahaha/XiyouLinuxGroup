#ifndef FTP_COMMANDS_H
#define FTP_COMMANDS_H

#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>

// FTP Response Code definitions (RFC 959)
enum class FTPResponseCode {
    READY_FOR_NEW_USER = 220,
    USER_LOGGED_IN = 230,
    PASSWORD_NEEDED = 331,
    ENTERING_PASSIVE_MODE = 227,
    FILE_STATUS_OK = 150,
    COMMAND_OK = 200,
    TRANSFER_COMPLETE = 226,
    CLOSING_CONTROL = 221,
    NOT_IMPLEMENTED = 502,
    SERVICE_NOT_AVAILABLE = 421
};

// FTP Commands
enum class FTPCommand {
    USER, PASS, PASV, LIST, RETR, STOR, 
    CWD, PWD, QUIT, UNKNOWN, REST, SYST, FEAT, PORT, EPSV, TYPE, SIZE, MDTM
};

class FTPProtocol {
public:
    static FTPCommand parseCommand(const std::string& cmd) {
        static const std::unordered_map<std::string, FTPCommand> cmdMap = {
            {"USER", FTPCommand::USER},
            {"PASS", FTPCommand::PASS},
            {"PASV", FTPCommand::PASV},
            {"LIST", FTPCommand::LIST},
            {"RETR", FTPCommand::RETR},
            {"STOR", FTPCommand::STOR},
            {"CWD", FTPCommand::CWD},
            {"PWD", FTPCommand::PWD},
            {"QUIT", FTPCommand::QUIT},
            {"REST", FTPCommand::REST},
            {"SYST", FTPCommand::SYST},
            {"FEAT", FTPCommand::FEAT},
            {"PORT", FTPCommand::PORT},
            {"EPSV", FTPCommand::EPSV},
            {"TYPE", FTPCommand::TYPE},
            {"SIZE", FTPCommand::SIZE},
            {"MDTM", FTPCommand::MDTM},
        };
        
        std::string upperCmd;
        upperCmd.reserve(cmd.size());
        for (char c : cmd) {
            upperCmd += std::toupper(static_cast<unsigned char>(c));
        }
        
        auto it = cmdMap.find(upperCmd);
        return it != cmdMap.end() ? it->second : FTPCommand::UNKNOWN;
    }
    
    static std::string getResponse(FTPResponseCode code, const std::string& msg = "") {
        return std::to_string(static_cast<int>(code)) + " " + msg + "\r\n";
    }
};

#endif // FTP_COMMANDS_H