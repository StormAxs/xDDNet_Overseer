#ifndef RCON_PARSE_H
#define RCON_PARSE_H

#include <string>
#include <regex>
#include <base/system.h> // For dbg_msg

// Structure to hold the parsed RCON log entry information
struct RconLogEntry {
    int id;               // Unique identifier for the player
    std::string addr;     // Address of the player
    std::string name;     // Name of the player
    int client;           // Client ID
    std::string secure;   // Security status (yes/no)
    int flags;            // Flags associated with the player
    std::string dnsbl;    // DNS blacklist status (white/black/grey)
    std::string key;      // Optional key
};

class CRconParse {
public:
    // Parses player info from a given text input
    RconLogEntry ParsePlayerInfo(const std::string& text);

    // Validates a single RCON log line
    bool ValidateRconLogLine(const std::string& line);
};

#endif // RCON_PARSE_H