#ifndef RCON_PARSE_H
#define RCON_PARSE_H

#include <string>
#include <regex>
#include <base/system.h>
#include <engine/shared/protocol.h>
#include <game/client/component.h>

struct ClientInfo {
    short id;
    std::string addr;
    std::string name;
    std::string kog_id;
    short client;
    std::string secure;
    short flags;
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
        bool rconAuthenticated = false;
        ClientInfo ClientsInfo[MAX_CLIENTS];
        const ClientInfo ClientInfoPayload = {-1, "0.0.0.0", "0", "0", 0, "", 0, "", ""};

        bool IsValidLogEntry(const char* text);
        ClientInfo ParseRconLine(const char* line);
};

#endif // RCON_PARSE_H