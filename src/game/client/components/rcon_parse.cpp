#include "rcon_parse.h"

// Parses player info from a given text input
RconLogEntry CRconParse::ParsePlayerInfo(const std::string& text) {
    std::regex regex(R"((\w+)='([^']+)'|(\w+)=(\S+))");
    std::smatch match;

    RconLogEntry entry = {0, "1.1.1.1", "", 0, "", 0, "", ""}; // Initialize with default values

    auto it = text.cbegin();
    while (std::regex_search(it, text.cend(), match, regex)) {
        std::string key = match[1].str().empty() ? match[3].str() : match[1].str();
        std::string value = match[1].str().empty() ? match[4].str() : match[2].str();

        // Remove single quotes from values if they exist
        if (value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.size() - 2);
        }

        // Assign values to the entry based on the key
        if (key == "id") {
            entry.id = std::stoi(value);
        } else if (key == "addr") {
            entry.addr = value;
        } else if (key == "name") {
            entry.name = value;
        } else if (key == "client") {
            entry.client = std::stoi(value);
        } else if (key == "secure") {
            entry.secure = value;
        } else if (key == "flags") {
            entry.flags = std::stoi(value);
        } else if (key == "dnsbl") {
            entry.dnsbl = value;
        } else if (key == "key") {
            entry.key = value;
        }

        it = match[0].second; // Move iterator forward
        dbg_msg("RCON", "%s", text.c_str());
    }

    return entry;
}

// Validates a single RCON log line
bool CRconParse::ValidateRconLogLine(const std::string& line) {
    // Basic validation: check if the line is not empty and contains expected format
    return !line.empty() && std::regex_match(line, std::regex(R"(\w+=\S+)"));
}