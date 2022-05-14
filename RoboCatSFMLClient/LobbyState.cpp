#include "ColorJumpClientPCH.hpp"

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
	m_start_countdown_timer(5),
	m_failed_connection_timer(0),
	m_game_started(false),
	m_connected(false),
	m_ready(false),
	m_local_id(-1),
	m_is_connecting(true)
{
	const SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(
		PlayerDataManager::sInstance->GetIP());

	NetworkManagerClient::StaticInit(*serverAddress, PlayerDataManager::sInstance->GetName());

	for (int i = 1; i < 9; ++i)
	{
		m_team_selections.try_emplace(i, std::vector<int>());
	}

	CreateUI();
}

void LobbyState::Unready()
{
	if (m_ready)
	{
		m_ready = !m_ready;
		NetworkManagerClient::sInstance->UpdateReady(m_ready);
	}
}

void LobbyState::HandleReady()
{
	m_ready = !m_ready;
	NetworkManagerClient::sInstance->UpdateReady(m_ready);
}

bool LobbyState::IsValidTeam() const
{
	return NetworkManagerClient::sInstance->GetTeamID() != 0 && !m_start_countdown && !
		m_game_started;
}

bool LobbyState::IsInTeam() const
{
	return NetworkManagerClient::sInstance->GetTeamID() != 0 && !m_start_countdown;
}

void LobbyState::HandleBackButton()
{
	SendClientDisconnect();
	m_local_id = NetworkManagerClient::sInstance->GetPlayerId();
}

void LobbyState::HandleTeamChoice(const int team_id)
{
	if (TeamHasPlace(team_id) || team_id == 0)
	{
		if (team_id == 0 || m_team_selections[team_id].empty())
		{
			NetworkManagerClient::sInstance->UpdateTeam(team_id, EColorType::kBlue);
		}
		else
		{
			NetworkManagerClient::sInstance->UpdateTeam(team_id, EColorType::kRed);
		}
	}
}

bool LobbyState::TeamHasPlace(const int team_id)
{
	if (m_team_selections[team_id].size() < 2)
	{
		return true;
	}

	return false;
}

void LobbyState::SendClientDisconnect()
{
	NetworkManagerClient::sInstance->SetState(
		NetworkManagerClient::NetworkClientState::NCS_Quit);
}

void LobbyState::SendPlayerName() const
{
	NetworkManagerClient::sInstance->UpdateName(m_player_input_name);
}

void LobbyState::CreateUI()
{
	int x = WindowManager::sInstance->getSize().x / 2;
	int y = WindowManager::sInstance->getSize().y / 2;

	Utility::CreateLabel(m_failed_connection_text, x, y, "Attempting to connect...", 35);
	Utility::CentreOrigin(m_failed_connection_text->GetText());
	m_gui_fail_container.Pack(m_failed_connection_text);

	std::shared_ptr<GUI::Label> label;
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

	Utility::CreateButton(button, UNPAIRED_POS_X, FOOTER_POS_Y, "Ready",
	                      HandleReadyPressed(), IsValidTeamPresent());
	m_gui_container.Pack(button);

	Utility::CreateButton(button, TEAM_COL_1_POS_X, FOOTER_POS_Y, "Leave Team",
	                      HandleLeaveTeamButtonPress(), IsInATeam());
	m_gui_container.Pack(button);

	Utility::CreateButton(button, TEAM_COL_2_POS_X + 150, FOOTER_POS_Y, "Leave",
	                      HandleBackButtonPressed());
	m_gui_container.Pack(button);

	Utility::CreateLabel(label, UNPAIRED_POS_X, FOOTER_POS_Y + 15,
	                     "Game starts in...",
	                     30, false);
	label->SetDrawPredicate([this] { return m_start_countdown; });
	m_gui_container.Pack(label);

	Utility::CreateLabel(m_start_countdown_label, UNPAIRED_POS_X + 250, FOOTER_POS_Y + 15,
	                     std::to_string(static_cast<int>(m_start_countdown_timer)), 30, false);
	m_start_countdown_label->SetDrawPredicate([this] { return m_start_countdown; });
	m_gui_container.Pack(m_start_countdown_label);
}

void LobbyState::Draw()
{
	WindowManager::sInstance->clear(sf::Color(50, 78, 161, 255));

	if (m_connected)
	{
		WindowManager::sInstance->draw(m_gui_container);
	}
	else
	{
		WindowManager::sInstance->draw(m_gui_fail_container);
	}
}

bool LobbyState::Update(const float dt)
{
	NetworkManagerClient::sInstance->ProcessIncomingPackets();
	NetworkManagerClient::sInstance->SendOutgoingPackets();

	if (m_is_connecting)
	{
		if (m_failed_connection_timer >= 5.f)
		{
			m_is_connecting = false;
			m_failed_connection_text->SetText("No servers available");
			Utility::CentreOrigin(m_failed_connection_text->GetText());
			m_failed_connection_timer = 0;
		}
	}


	if (m_connected)
	{
		if (m_start_countdown)
		{
			if (m_start_countdown_timer > 0)
			{
				m_start_countdown_timer -= dt;
				m_start_countdown_label->SetText(
					std::to_string(static_cast<int>(m_start_countdown_timer)));
			}
		}
	}

	if (!m_is_connecting && !m_connected)
	{
		if (m_failed_connection_timer >= 10.f)
		{
			RequestStackClear();
			RequestStackPush(StateID::kMenu);
		}
	}

	m_failed_connection_timer += dt;

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
				m_player_input_name += static_cast<char>(event.text.unicode);
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
		InputManager::sInstance->SetPassFocus(true);
	}
	else if (event.type == sf::Event::LostFocus)
	{
		InputManager::sInstance->SetPassFocus(false);
	}

	return false;
}

void LobbyState::OnStackPopped()
{
	State::OnStackPopped();
}

void LobbyState::MovePlayer(int player_id, const int team_id)
{
	RemovePlayerFromTeam(player_id);

	m_team_selections[team_id].emplace_back(player_id);

	m_player_team_selection[player_id] = team_id;

	UpdateTeamMembers(team_id);
}

void LobbyState::RemovePlayer(const int player_id)
{
	if (player_id == m_local_id)
	{
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
	}

	RemovePlayerFromTeam(player_id);
	
	m_player_team_selection.erase(player_id);
	m_gui_container.Pull(m_players[player_id]);
	m_players[player_id].reset();
	m_players.erase(player_id);
}


void LobbyState::AddPlayer(const int id, const std::string& label_text, const bool ready)
{
	GUI::Label::Ptr label;
	Utility::CreateLabel(label, UNPAIRED_POS_X, UNPAIRED_POS_Y + 30 * id,
	                     label_text, 20, false);

	if (m_players.try_emplace(id, label).second)
	{
		m_gui_container.Pack(label);
		m_player_team_selection.try_emplace(id, 0);
	}
}

sf::Vector2f LobbyState::GetTeamPos(const int i)
{
	const int y = TEAM_POS_Y + TEAM_BUTTON_GAP * ((i - 1 - (i - 1) % 2) / 2);
	const int x = i % 2 == 0 ? TEAM_COL_2_POS_X : TEAM_COL_1_POS_X;

	return {static_cast<float>(x), static_cast<float>(y)};
}

void LobbyState::UpdateTeamMembers(const int team_id)
{
	const sf::Vector2f pos = GetTeamPos(team_id);
	
	int i = 0;
	const vector<int> team = m_team_selections[team_id];
	for (int player_id : team)
	{
		float y = pos.y;
		if (i != 0)
		{
			y += 85;
		}
		else
		{
			y += 60;
		}

		m_players[player_id]->setPosition(pos.x, y);

		if (player_id == NetworkManagerClient::sInstance->GetPlayerId())
		{
			if (i == 0)
			{
				NetworkManagerClient::sInstance->UpdateColor(EColorType::kBlue);
				//m_players[player_id]->SetTextColor(sf::Color::Blue);
			}
			else
			{
				NetworkManagerClient::sInstance->UpdateColor(EColorType::kRed);
				/*m_players[player_id]->SetTextColor(sf::Color::Red);*/
			}
		}

		UpdateNameColor(player_id);

		i++;
	}
}


void LobbyState::RemovePlayerFromTeam(const int player_id)
{
	const int team_id = m_player_team_selection[player_id];
	if (team_id != 0)
	{
		m_team_selections[team_id].erase(
			std::remove(m_team_selections[team_id].begin(),
			            m_team_selections[team_id].end(), player_id),
			m_team_selections[team_id].end());
	}

	if (!m_team_selections[team_id].empty())
	{
		UpdateTeamMembers(team_id);
	}
}

void LobbyState::MovePlayerBack(const int player_id)
{
	RemovePlayerFromTeam(player_id);
	m_players[player_id]->setPosition(GetUnpairedPos(player_id));
	m_player_team_selection[player_id] = 0;
	UpdateNameColor(player_id);
}

void LobbyState::SetName(const int player_id, const string& name)
{
	m_players[player_id]->SetText(name);
}

void LobbyState::Start()
{
	if (m_team_selections[m_player_team_selection[NetworkManagerClient::sInstance->GetPlayerId()]].
		size() == 2)
	{
		m_game_started = true;
		RequestStackClear();
		RequestStackPush(StateID::kNetworkGame);
		return;
	}

	RequestStackPop();
	RequestStackPush(StateID::kMenu);
}

void LobbyState::UpdateNameColor(const int player_id)
{
	const int player_team_id = m_player_team_selection[player_id];

	if (player_team_id == 0)
	{
		m_players[player_id]->SetTextColor(sf::Color::White);
		return;
	}

	for (int i = 0; i < m_team_selections[player_team_id].size(); i++)
	{
		if (m_team_selections[player_team_id][i] == player_id)
			m_players[player_id]->SetTextColor(i == 0 ? sf::Color::Blue : sf::Color::Red);
	}
}

void LobbyState::SetReady(const int player_id, const bool ready)
{
	if(ready)
	{
		//Player is ready -> change name color to green
		m_players[player_id]->SetTextColor(sf::Color::Green);
	}
	else
	{
		//Player is unready -> change name color to character color (which is based on the name slot)
		UpdateNameColor(player_id);
	}
}

void LobbyState::StartCountDown()
{
	m_start_countdown = true;
}

void LobbyState::HandleConnected()
{
	m_is_connecting = false;
	m_connected = true;
}

void LobbyState::Quit() const
{
	if (m_failed_connection_timer > 10.f)
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}
}

sf::Vector2f LobbyState::GetUnpairedPos(const int i)
{
	constexpr int x = UNPAIRED_POS_X;
	const int y = UNPAIRED_POS_Y + 30 * i;

	return {static_cast<float>(x), static_cast<float>(y)};
}

void LobbyState::Debug(const std::string& message) const
{
	const string in_format = "Lobby: " + message;
	LOG(in_format.c_str(), 0)
}
