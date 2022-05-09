#pragma once
#include "WorldClient.hpp"

class MultiplayerGameState final : public State
{
public:
	MultiplayerGameState();

	void OnStackPopped() override;

	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void Draw() override;

private:
	void SendClientDisconnect(sf::Int8 identifier) const;
	void HandleClientUpdate(InputMemoryBitStream& packet) const;
	void SpawnClientPlayer(sf::Int8 identifier, sf::Int8 team_id, sf::Int8 color, std::string name);
	void SpawnGhostPlayer(sf::Int8 identifier, sf::Int8 team_id, sf::Int8 color, const std::string& name);
	void HandlePlayerDisconnect(InputMemoryBitStream& packet);
	void HandleUpdatePlatformColors(InputMemoryBitStream& packet);
	void HandleMission(InputMemoryBitStream& packet);
	void HandleTeamRespawn(InputMemoryBitStream& packet) const;
	void HandleTeamCheckpointSet(InputMemoryBitStream& packet);
	void HandlePacket(sf::Int8 packet_type, InputMemoryBitStream& packet);

	static void Debug(const std::string& message);

	WorldClient* m_world_client;
	/*std::map<int, PlayerPtr> m_players;*/
	sf::Int8 m_local_player_identifier {};
};
