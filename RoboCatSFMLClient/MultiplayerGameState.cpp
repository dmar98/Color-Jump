#include "RoboCatClientPCH.hpp"

MultiplayerGameState::MultiplayerGameState()
	: m_world_client(dynamic_cast<WorldClient*>(World::sInstance.get())),
	  m_game_over(false),
	  m_completion_time(0)
{
	//Load the Level
	m_world_client->LoadLevel();
	m_world_client->SetCamera(WindowManager::sInstance->getDefaultView());
}

void MultiplayerGameState::OnStackPopped()
{
	m_world_client->RemoveAllGameObjects();
}

bool MultiplayerGameState::Update(const float dt)
{
	NetworkManagerClient::sInstance->ProcessIncomingPackets();
	NetworkManagerClient::sInstance->SendOutgoingPackets();

	m_world_client->Update();

	if (!m_game_over)
		m_completion_time += dt;

	return false;
}

bool MultiplayerGameState::HandleEvent(const sf::Event& event)
{
	return false;
}

void MultiplayerGameState::Draw()
{
	//Draw the Level using the worlds camera (the client player's camera)
	RenderManager::sInstance->RenderComponents(m_world_client->GetCamera());
}

void MultiplayerGameState::SendClientDisconnect(sf::Int8 identifier) const
{
}

void MultiplayerGameState::HandleClientUpdate(InputMemoryBitStream& packet) const
{
}

void MultiplayerGameState::SpawnClientPlayer(const int player_id, const int team_id,
                                             const EColorType color,
                                             const std::string& name) const
{
	const auto character = m_world_client->AddCharacter(player_id, color);
	character->SetTeamIdentifier(team_id);
	character->SetName(name);

	LeaderBoardManager::sInstance->AddPlayer(team_id, player_id, name);
	/*m_players[player_id].reset(new Player(m_socket, player_id, GetContext().m_keys1));*/
	m_world_client->UpdateCharacterTransparencies(team_id);
}

void MultiplayerGameState::SpawnGhostPlayer(const int player_id, const int team_id,
                                            const EColorType color,
                                            const std::string& name) const
{
	const auto ghost = m_world_client->AddGhostCharacter(player_id, color);
	ghost->SetTeamIdentifier(team_id);
	ghost->SetName(name);

	LeaderBoardManager::sInstance->AddPlayer(team_id, player_id, name);
	/*m_players[player_id].reset(new Player(m_socket, player_id, nullptr));*/
}

void MultiplayerGameState::HandlePlayerDisconnect(InputMemoryBitStream& packet)
{
}

void MultiplayerGameState::HandlePlatformChange(const int player_id, const int platform_id,
                                                const EPlatformType platform_color) const
{
	const auto client_char = m_world_client->GetClientCharacter();
	const auto send_char = m_world_client->GetCharacter(player_id);

	if (client_char->GetTeamIdentifier() == send_char->GetTeamIdentifier())
	{
		//Update the current platform on the character that belongs to your team mate
		//(this will later be needed to check for checkpoints and goal tiles)
		if (client_char->GetPlayerID() != player_id)
		{
			m_world_client->SetPlatformOnCharacter(send_char, platform_id);
			m_world_client->SetTeammate(send_char);
		}

		m_world_client->UpdatePlatform(send_char->GetPlayerID(), platform_id, platform_color);
	}
}

void MultiplayerGameState::HandleGameEnd(const int team_id)
{
	if (!m_game_over && team_id == NetworkManagerClient::sInstance->GetTeamID())
	{
		//Leaderboard empty => you are first place (you won)
		if (LeaderBoardManager::sInstance->GetLeaderboard().empty())
			RequestStackPush(StateID::kMultiplayerWin);
		else
			RequestStackPush(StateID::kMultiplayerLose);

		m_game_over = true;
	}
	LeaderBoardManager::sInstance->AddToLeaderboard(team_id, m_completion_time);
}

void MultiplayerGameState::HandleTeamRespawn(const int team_id) const
{
	if (m_world_client->GetClientCharacter()->GetTeamIdentifier() == team_id)
	{
		m_world_client->RespawnClientCharacter();
	}
}

void MultiplayerGameState::HandleTeamCheckpointSet(const int team_id, const int platform_id) const
{
	if (m_world_client->GetClientCharacter()->GetTeamIdentifier() == team_id)
	{
		m_world_client->SetCheckpointToPlatformWithID(platform_id);
	}
}

void MultiplayerGameState::HandlePacket(sf::Int8 packet_type, InputMemoryBitStream& packet)
{
}

void MultiplayerGameState::Debug(const std::string& message)
{
	const string in_format = "MultiplayerGameState: " + message;
	LOG(in_format.c_str(), 0)
}
