#include "rcon_parse.h"

#include <game/client/gameclient.h>
#include <game/client/components/console.h>

void CGameConsoleParse::OnInit() {
    for(short i = 0; i < MAX_CLIENTS; i++)ClientsInfo[i] = ClientInfoPayload;
}

void CGameConsoleParse::RconAuthenticated(bool status) {
    m_rconAuthenticated = status;
}

void CGameConsoleParse::Refresh() {
    if(!m_rconAuthenticated)return;

    CGameConsole::CInstance *rconConsole = m_pClient->m_GameConsole.ConsoleForType(CGameConsole::CONSOLETYPE_REMOTE);
    CStaticRingBuffer<CGameConsole::CInstance::CBacklogEntry, 1024 * 1024, CRingBufferBase::FLAG_RECYCLE> rconConsoleLog = rconConsole->m_Backlog;

    rconConsole->ExecuteLine("status");

    for(CGameConsole::CInstance::CBacklogEntry *pEntry = rconConsoleLog.get(); pEntry; pEntry = rconConsoleLog.Next(pEntry))
    {        
        if(!IsValidLogEntry(pEntry->m_aText))continue; // Validate line

        ClientInfo pClient = ParseRconLine(pEntry->m_aText);
        ClientsInfo[pClient.id] = pClient;
    }    
}

ClientInfo CGameConsoleParse::GetClientById(short ClientId) {
    if(ClientId>MAX_CLIENTS || ClientId<0)return ClientInfoPayload;
    return ClientsInfo[ClientId];
}

// Регулярное выражение для валидации строки
bool CGameConsoleParse::IsValidLogEntry(const char* text) { 
    std::regex logPattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} I server: id=(?:[0-6]?\d|[1-9][0-9]) (?:\([a-f0-9-]+\) )?addr=[^ ]+ name='[^']*' (?:kog_id=\d+ )?client=\d+ secure=(yes|no) flags=([0-9]{1,3}|[1-9][0-9]{1,2}|1000)(?:\s+dnsbl=[a-zA-Z]+)?(?:\s+key=[^ ]+(?:\s+\([^)]+\))?)?$)");
    return std::regex_match(text, logPattern);
}

// Регулярное выражение для парсинга строк
inline ClientInfo CGameConsoleParse::ParseRconLine(const char* line) {
    std::regex regex(R"((\w+)=('[^']*'|[^ ]+))");
    std::smatch match;

    std::string lineStr(line);

    ClientInfo result = ClientInfoPayload;

    // Итерация по всем совпадениям
    auto it = lineStr.cbegin();
    while (std::regex_search(it, lineStr.cend(), match, regex)) {
        std::string key = match[1].str(); // Имя параметра
        std::string value = match[2].str(); // Значение параметра

        // Удаляем одинарные кавычки из значений, если они есть
        if (value.front() == '\'' && value.back() == '\'') value = value.substr(1, value.size() - 2);
        
        if(key=="id")result.id = atoi(value.c_str());
        else if(key=="addr")result.addr = value;
        else if(key=="name")result.name = value;
        else if(key=="kog_id")result.kog_id = atoi(value.c_str());
        else if(key=="client")result.client = atoi(value.c_str());
        else if(key=="secure")result.secure = value;
        else if(key=="flags")result.flags = atoi(value.c_str());
        else if(key=="dnsbl")result.dnsbl = value;
        else if(key=="key")result.key = value;
        
        it = match[0].second; // Обновляем итератор
    }
    return result;
}