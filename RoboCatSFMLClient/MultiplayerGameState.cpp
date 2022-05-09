#include "RoboCatClientPCH.hpp"

MultiplayerGameState::MultiplayerGameState()
	:m_world_client(dynamic_cast<WorldClient*>(World::sInstance.get()))
{
	//Load the Level
	m_world_client->LoadLevel();
	m_world_client->SetCamera(WindowManager::sInstance->getDefaultView());
}

void MultiplayerGameState::OnStackPopped()
{
	m_world_client->RemoveAllGameObjects();
}

bool MultiplayerGameState::Update(float dt)
{
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

void MultiplayerGameState::SpawnClientPlayer(sf::Int8 identifier, sf::Int8 team_id, sf::Int8 color, const std::string name)
{
	Debug("Self spawn.");

	m_local_player_identifier = identifier;
	const auto character = m_world_client->AddCharacter(identifier, color, true);
	character->SetTeamIdentifier(team_id);
	character->SetName(name);

	LeaderBoardManager::sInstance->AddPlayer(team_id, identifier, name);
	/*m_players[identifier].reset(new Player(m_socket, identifier, GetContext().m_keys1));*/
	m_world_client->UpdateCharacterTransparencies(team_id);
}

void MultiplayerGameState::SpawnGhostPlayer(sf::Int8 identifier, sf::Int8 team_id, sf::Int8 color, const std::string& name)
{
	const auto ghost = m_world_client->AddGhostCharacter(identifier, color);
	ghost->SetTeamIdentifier(team_id);
	ghost->SetName(name);

	LeaderBoardManager::sInstance->AddPlayer(team_id, identifier, name);
	/*m_players[identifier].reset(new Player(m_socket, identifier, nullptr));*/
}

void MultiplayerGameState::HandlePlayerDisconnect(InputMemoryBitStream& packet)
{
}

void MultiplayerGameState::HandleUpdatePlatformColors(InputMemoryBitStream& packet)
{
}

void MultiplayerGameState::HandleMission(InputMemoryBitStream& packet)
{
}

void MultiplayerGameState::HandleTeamRespawn(InputMemoryBitStream& packet) const
{
}

void MultiplayerGameState::HandleTeamCheckpointSet(InputMemoryBitStream& packet)
{
}

void MultiplayerGameState::HandlePacket(sf::Int8 packet_type, InputMemoryBitStream& packet)
{
}

void MultiplayerGameState::Debug(const std::string& message)
{
	Utility::Debug("MultiplayerGameState: " + message);
}

