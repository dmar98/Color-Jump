#include "RoboCatClientPCH.hpp"

constexpr int UNPAIRED_POS_X = 100;
constexpr int TITLE_POS_Y = 30;
constexpr int TITLE_SIZE = 100;
constexpr int TEAM_POS_Y = 260;
constexpr int TEAM_COL_1_POS_X = 550;
constexpr int TEAM_COL_2_POS_X = 950;
constexpr int TEAM_BUTTON_GAP = 135;
constexpr int FOOTER_POS_Y = 850;
constexpr int UNPAIRED_POS_Y = TEAM_POS_Y - 20;

LobbyState::LobbyState():
	m_player_input_name(PlayerDataManager::sInstance->GetName()),
	m_start_countdown(false),
	m_start_countdown_timer(sf::seconds(5.f)),
	m_player_id(-1),
	m_game_started(false),
	m_connected(true)
{
	const SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(
		PlayerDataManager::sInstance->GetIP());

	NetworkManagerClient::StaticInit(*serverAddress, PlayerDataManager::sInstance->GetName());

	for (int i = 0; i < 8; ++i)
	{
		m_team_selections.try_emplace(i, std::vector<int>());
	}

	CreateUI();
}

auto LobbyState::HandleTutorialPress() const
{
	return [this]
	{
		RequestStackPush(StateID::kTutorial);
	};
}

auto LobbyState::HandleTeamButtonPressed(int id)
{
	return [this, id]
	{
		HandleTeamChoice(id);
	};
}

auto LobbyState::HandleStartGamePressed() const
{
	return [this]
	{
	};
}

auto LobbyState::IsHostAndInTeam()
{
	return [this]
	{
		return m_team_selections[m_player_team_selection[m_player_id]].size() == 2 && !
			m_start_countdown &&
			!m_game_started;
	};
}

auto LobbyState::HandleLeaveTeamButtonPress()
{
	return [this]
	{
		HandleTeamChoice(0);
	};
}

auto LobbyState::IsInATeam()
{
	return [this] { return m_player_team_selection[m_player_id] != 0; };
}

auto LobbyState::HandleBackButtonPressed() const
{
	return [this]
	{
		SendClientDisconnect();
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
	};
}

std::map<int, std::vector<int>>::mapped_type LobbyState::GetPlayerTeam(
	const int player_id)
{
	return m_team_selections[player_id];
}

void LobbyState::HandleTeamChoice(const int team_id)
{
	NetworkManagerClient::sInstance->SetTeamID(team_id);
	const int player_id = NetworkManagerClient::sInstance->GetPlayerId();
	if (GetPlayerTeam(player_id).empty())
	{
		NetworkManagerClient::sInstance->SetPlayerColor(0);
	}
	else
	{
		NetworkManagerClient::sInstance->SetPlayerColor(1);
	}


	NetworkManagerClient::sInstance->SetState(
		NetworkManagerClient::NetworkClientState::NCS_Team_Change);
}

void LobbyState::SendClientDisconnect()
{
	NetworkManagerClient::sInstance->SetState(
		NetworkManagerClient::NetworkClientState::NCS_Quit);
}

void LobbyState::SendPlayerName() const
{
	NetworkManagerClient::sInstance->SetName(m_player_input_name);

	NetworkManagerClient::sInstance->SetState(
		NetworkManagerClient::NetworkClientState::NCS_Player_Update);
}

void LobbyState::CreateUI()
{
	int x = WindowManager::sInstance->getSize().x / 2;
	int y = WindowManager::sInstance->getSize().y / 2;

	std::shared_ptr<GUI::Label> label;
	Utility::CreateLabel(label, x, y, "Attempting to connect...", 35, false);
	m_gui_fail_container.Pack(label);


	Utility::CreateLabel(label, UNPAIRED_POS_X, TITLE_POS_Y, "Lobby", TITLE_SIZE, false);
	m_gui_container.Pack(label);

	std::shared_ptr<GUI::Button> button;
	Utility::CreateButton(m_change_name_button, TEAM_COL_1_POS_X, TITLE_POS_Y + 10, "Name", true);
	m_gui_container.Pack(m_change_name_button);

	Utility::CreateLabel(m_current_name_label, TEAM_COL_1_POS_X + 215, TITLE_POS_Y + 25,
	                     m_player_input_name, 20, false);
	m_gui_container.Pack(m_current_name_label);

	Utility::CreateButton(button, TEAM_COL_1_POS_X, TITLE_POS_Y + 85,
	                      "How to Play", HandleTutorialPress());
	m_gui_container.Pack(button);

	Utility::CreateLabel(label, UNPAIRED_POS_X, TEAM_POS_Y - 50, "Unpaired Players", 30, false);
	m_gui_container.Pack(label);

	for (sf::Int8 id = 1; id <= 8; ++id)
	{
		y = TEAM_POS_Y + TEAM_BUTTON_GAP * ((id - 1 - (id - 1) % 2) / 2);
		x = id % 2 == 0 ? TEAM_COL_2_POS_X : TEAM_COL_1_POS_X;
		auto basic_string = "Team " + std::to_string(id);
		Utility::CreateButton(button, x, y, basic_string, HandleTeamButtonPressed(id));
		m_gui_container.Pack(button);
	}

	Utility::CreateButton(button, UNPAIRED_POS_X, FOOTER_POS_Y, "Start game",
	                      HandleStartGamePressed(), IsHostAndInTeam());
	m_gui_container.Pack(button);

	Utility::CreateButton(button, TEAM_COL_1_POS_X, FOOTER_POS_Y, "Leave Team",
	                      HandleLeaveTeamButtonPress(), IsInATeam());
	m_gui_container.Pack(button);

	Utility::CreateButton(button, TEAM_COL_2_POS_X + 150, FOOTER_POS_Y, "Leave",
	                      HandleBackButtonPressed());
	m_gui_container.Pack(button);

	Utility::CreateLabel(label, UNPAIRED_POS_X, FOOTER_POS_Y + 15,
	                     "Game starts in...",
	                     30);
	label->SetDrawPredicate([this] { return m_start_countdown; });
	m_gui_container.Pack(label);

	Utility::CreateLabel(m_start_countdown_label, UNPAIRED_POS_X + 250, FOOTER_POS_Y + 15,
	                     std::to_string(m_start_countdown_timer.asSeconds()), 30);
	m_start_countdown_label->SetDrawPredicate([this] { return m_start_countdown; });
	m_gui_container.Pack(m_start_countdown_label);
}

void LobbyState::Draw()
{
	WindowManager::sInstance->clear(sf::Color(0, 37, 97, 255));

	if (m_connected)
	{
		WindowManager::sInstance->draw(m_gui_container);
	}
	else
	{
		WindowManager::sInstance->draw(m_gui_fail_container);
	}
}

bool LobbyState::Update(float dt)
{
	NetworkManagerClient::sInstance->ProcessIncomingPackets();
	NetworkManagerClient::sInstance->SendOutgoingPackets();
	return true;
}

bool LobbyState::HandleEvent(const sf::Event& event)
{
	if (m_game_started)
		return false;

	if (m_change_name_button->IsActive())
	{
		//Name Input
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
		{
			m_change_name_button->Deactivate();
			PlayerDataManager::sInstance->SetName(m_player_input_name);
			PlayerDataManager::sInstance->Save();
			SendPlayerName();
		}
		else if (event.type == sf::Event::TextEntered)
		{
			if (event.text.unicode == '\b')
			{
				if (!m_player_input_name.empty())
					m_player_input_name.erase(m_player_input_name.size() - 1, 1);
			}
			else if (event.text.unicode != '\n' && event.text.unicode != '\r')
			{
				m_player_input_name += event.text.unicode;
				m_player_input_name = m_player_input_name.substr(0, 15);
			}

			m_current_name_label->SetText(m_player_input_name);
		}
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}

	if (event.type == sf::Event::GainedFocus)
	{
		// todo
	}
	else if (event.type == sf::Event::LostFocus)
	{
		// todo
	}

	return false;
}

void LobbyState::OnStackPopped()
{
	State::OnStackPopped();
}

void LobbyState::MovePlayer(int player_id, int team_id)
{
	if (m_player_team_selection[player_id] != 0)
	{
		m_team_selections[m_player_team_selection[player_id]].erase(
			std::remove(m_team_selections[m_player_team_selection[player_id]].begin(),
			            m_team_selections[m_player_team_selection[player_id]].end(), player_id),
			m_team_selections[m_player_team_selection[player_id]].end());
	}

	m_team_selections[team_id].emplace_back(player_id);

	const sf::Vector2f pos = GetTeamPos(team_id);
	float y = pos.y;

	if (m_team_selections[team_id].front() != player_id)
	{
		y += 85;
	}
	else
	{
		y += 60;
	}

	m_players[player_id]->setPosition(pos.x, y);

	m_player_team_selection[player_id] = team_id;
}

void LobbyState::AddPlayer(const int id, const std::string& label_text)
{
	GUI::Label::Ptr label;
	Utility::CreateLabel(label, UNPAIRED_POS_X, UNPAIRED_POS_Y + 30 * id,
	                     label_text, 20, false);
	m_gui_container.Pack(label);
	m_players.try_emplace(id, label);
	m_player_team_selection.try_emplace(id, 0);
}

sf::Vector2f LobbyState::GetTeamPos(const int i)
{
	const int y = TEAM_POS_Y + TEAM_BUTTON_GAP * ((i - 1 - (i - 1) % 2) / 2);
	const int x = i % 2 == 0 ? TEAM_COL_2_POS_X : TEAM_COL_1_POS_X;

	return {static_cast<float>(x), static_cast<float>(y)};
}


void LobbyState::MovePlayerBack(const int id)
{
	if (m_player_team_selection[id] != 0)
	{
		m_team_selections[m_player_team_selection[id]].erase(
			std::remove(m_team_selections[m_player_team_selection[id]].begin(),
				m_team_selections[m_player_team_selection[id]].end(), id),
			m_team_selections[m_player_team_selection[id]].end());
	}

	m_players[id]->setPosition(GetUnpairedPos(id));
	m_player_team_selection[id] = 0;
}

sf::Vector2f LobbyState::GetUnpairedPos(const int i)
{
	constexpr int x = UNPAIRED_POS_X;
	const int y = UNPAIRED_POS_Y + 30 * i;
	

	return { static_cast<float>(x), static_cast<float>(y) };
}