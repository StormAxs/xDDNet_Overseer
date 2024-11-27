#ifndef RCON_PARSE_H
#define RCON_PARSE_H

#include <string>
#include <regex>
#include <base/system.h>
#include <engine/shared/protocol.h>
#include <game/client/component.h>

struct ClientInfo {
    signed short id;
    std::string addr;
    std::string name;
    unsigned long long kog_id;
    signed short client;
    bool secure;
    signed short flags;
    std::string dnsbl;
    std::string key;
};

class CGameConsoleParse: public CComponent {
public:
	virtual int Sizeof() const override { return sizeof(*this); }

	virtual void OnInit() override;

    public:
        void Refresh(); 
        void RconAuthenticated(bool status);

        ClientInfo GetClientById(short ClientId);



    private:
        bool m_rconAuthenticated = false;
        ClientInfo ClientsInfo[MAX_CLIENTS];
        const int maxEntries = 200; // Maximum number of entries to process
        const ClientInfo ClientInfoPayload = {
            MAX_CLIENTS,
            "0.0.0.0:00000",
            "",
            0,
            0,
            false,
            0,
            "",
            ""};

        bool IsValidLogEntry(const char* text);
        std::string extractIP(const std::string& input);
        ClientInfo ParseRconLine(const char* line);
};

#endif // RCON_PARSE_H