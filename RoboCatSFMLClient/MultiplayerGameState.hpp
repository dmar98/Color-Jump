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

	void HandleGameEnd(int team_id);
	void HandlePlatformChange(int player_id, int platform_id, EPlatformType platform_color) const;
	void HandleTeamRespawn(int team_id) const;
	void HandleTeamCheckpointSet(int team_id, int platform_id) const;
	void SpawnClientPlayer(int player_id, int team_id, EColorType color,
	                       const std::string& name);
	void SpawnGhostPlayer(int player_id, int team_id, EColorType color,
		const std::string& name);
	void UpdatePlayers(int player_id, float x, float y) const;

private:
	static void SendClientDisconnect();
	void HandlePlayerDisconnect(InputMemoryBitStream& packet);


	void HandlePacket(sf::Int8 packet_type, InputMemoryBitStream& packet);

	static void Debug(const std::string& message);

	WorldClient* m_world_client;
	/*std::map<int, PlayerPtr> m_players;*/
	bool m_game_over;
	float m_completion_time;

	using PlayerPtr = std::unique_ptr<Player>;
	std::map<int, PlayerPtr> m_players; 
};
