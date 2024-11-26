#include <base/math.h>
#include <base/system.h>

#include <engine/demo.h>
#include <engine/favorites.h>
#include <engine/friends.h>
#include <engine/ghost.h>
#include <engine/graphics.h>
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>
#include <engine/shared/localization.h>
#include <engine/textrender.h>

#include <game/generated/client_data.h>
#include <game/generated/protocol.h>

#include <game/client/animstate.h>
#include <game/client/components/countryflags.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>
#include <game/client/ui.h>
#include <game/client/ui_listbox.h>
#include <game/client/ui_scrollregion.h>
#include <game/localization.h>
#include "rcon_parse.h"
#include "menus.h"
#include "motd.h"
#include "voting.h"

#include "ghost.h"
#include <engine/keys.h>
#include <engine/storage.h>

#include <chrono>
#include <gmock/gmock-matchers.h>

using namespace FontIcons;
using namespace std::chrono_literals;


void CMenus::RenderServerOverseerPanel(CUIRect MainView) {
    MainView.Draw(ms_ColorTabbarActive, IGraphics::CORNER_B, 10.0f);
    MainView.Margin(20.0f, &MainView);
    CUIRect LeftView, RightView, Line, Rect;
    MainView.VSplitMid(&LeftView, &RightView, 10.f);

    static int s_SelectedPlayerIndex = -1; // Selected player index

    if(Client()->RconAuthed()) {


		// FIX THIS KOSTYL
		static bool isWorking = true;

		if(isWorking){
			m_pClient->m_GameConsoleParse.RconAuthenticated(true);
			m_pClient->m_GameConsoleParse.Refresh();

			isWorking = false;
		}

        // Players search
		LeftView.HSplitTop(20.f, &Line, &LeftView);
		Line.VSplitRight(20.f, &Line, &Rect);
		Line.VSplitRight(5.f, &Line, nullptr);
		static CLineInputBuffered<MAX_NAME_LENGTH> s_Input;
		s_Input.SetEmptyText(Localize("Player search. Type name or clan."));
		Ui()->DoEditBox(&s_Input, &Line, 16.0f);

		static CButtonContainer s_ButtonEditWidgets;
		if(DoButton_Menu(&s_ButtonEditWidgets, "X", 0, &Rect))
			s_Input.Clear();

		LeftView.HSplitTop(5.f, nullptr, &LeftView);

		// Clients list
		static CScrollRegion s_ScrollRegion;
		vec2 ScrollOffset(0.0f, 0.0f);
		CScrollRegionParams ScrollParams;
		ScrollParams.m_ScrollUnit = 120.0f;
		s_ScrollRegion.Begin(&LeftView, &ScrollOffset, &ScrollParams);
		LeftView.y += ScrollOffset.y;

		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			CGameClient::CClientData &Client = GameClient()->m_aClients[i];
			if(!Client.m_Active)
				continue;
			if(!s_Input.IsEmpty() && !str_find_nocase(Client.m_aName, s_Input.GetString()) && !str_find_nocase(Client.m_aClan, s_Input.GetString()))
				continue;

			LeftView.HSplitTop(30.f, &Line, &LeftView);
			LeftView.HSplitTop(5.f, nullptr, &LeftView);
			if(!s_ScrollRegion.AddRect(Line))
				continue;

			// Invisible button
			static CButtonContainer s_aButtons[MAX_CLIENTS];
			if(DoButton_Menu(&s_aButtons[i], "", 0, &Line, 0x0, 0, 0, 0, ColorRGBA(0.f, 0.f, 0.f, 0.2f)))
				s_SelectedPlayerIndex = i;

			// Background
			Line.Draw(ColorRGBA(1.f, 1.f, 1.f, s_SelectedPlayerIndex == i ? 0.4f : 0.2f), IGraphics::CORNER_ALL, 5);

			// Player skin
			Line.VSplitLeft(30.f, &Rect, &Line);
			CTeeRenderInfo RenderInfo = Client.m_RenderInfo;
			RenderInfo.m_Size = 25.f;

			vec2 OffsetToMid;
			CRenderTools::GetRenderTeeOffsetToRenderedTee(CAnimState::GetIdle(), &RenderInfo, OffsetToMid);
			const vec2 TeeRenderPos = vec2(Rect.x + Rect.h / 2, Rect.y + Rect.h / 2 + OffsetToMid.y);
			RenderTools()->RenderTee(CAnimState::GetIdle(), &RenderInfo, EMOTE_NORMAL, vec2(1, 0), TeeRenderPos);

			// Player name
			Line.HMargin(3.f, &Rect);
			Ui()->DoLabel(&Rect, Client.m_aName, 15.f, TEXTALIGN_LEFT);
			Line.HSplitBottom(12.f, nullptr, &Rect);
			Ui()->DoLabel(&Rect, Client.m_aClan, 8.f, TEXTALIGN_LEFT);
		}

		s_ScrollRegion.End();
	}


    // Display selected player information
	CUIRect addr;
    if(s_SelectedPlayerIndex != -1) {
    	CGameClient::CClientData &Client = GameClient()->m_aClients[s_SelectedPlayerIndex];
		ClientInfo pClient = m_pClient->m_GameConsoleParse.GetClientById(s_SelectedPlayerIndex);

		std::string pName = Client.m_aName;
        char aBuf[256];
        str_format(aBuf, sizeof(aBuf), "Selected Player: %s", Client.m_aName);
        Ui()->DoLabel(&RightView, aBuf, 10.0f, TEXTALIGN_LEFT);
    	char adBuf[256];
		RightView.HSplitTop(RightView.h, &RightView, &addr);
    	RightView.Draw(ColorRGBA(1.f, 0.f, 0.f, 0.f), IGraphics::CORNER_B, 10);
    	str_format(adBuf, sizeof(adBuf), "Selected addr: %s", pClient.addr.c_str());
    	Ui()->DoLabel(&addr, adBuf, 10.0f, TEXTALIGN_LEFT);

    }
}