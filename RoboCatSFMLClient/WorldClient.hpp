#pragma once
#include "CharacterClient.hpp"

class WorldClient : public World
{
public:
	WorldClient();

	void LoadLevel() override;
	void Update() override;

	void SetCamera(sf::View camera);
	sf::View& GetCamera();

	Character* AddCharacterWithColor(sf::Int8 identifier, EColorType color, sf::IntRect rect, Vector3 spawn_pos) override;
	Character* AddCharacter(sf::Int8 identifier, sf::Int8 color, bool is_client_player) override;
	Character* AddGhostCharacterWithColor(sf::Int8 identifier, EColorType color, const sf::IntRect& int_rect, Vector3 spawn_pos) override;
	Character* AddGhostCharacter(sf::Int8 identifier, sf::Int8 color) override;
	Character* GetClientCharacter() const;
	void SetTeammate(Character* character);
	Character* GetTeammate() const;

	void UpdateCharacterTransparencies(sf::Int8 team_id) const;
	void UpdateCharacterTransparencies() const;
	void RespawnClientCharacter() const;
	void SetCheckpointToPlatformWithID(sf::Int8 platform_id);

protected:
	void CheckClientCollisions() const;

private:
	void OnReachedCheckpoint() const;
	void OnReachedGoal() const;
	void OnClientPlayerDeath() const;

	sf::View m_camera;
	Platform* m_checkpoint;
	CharacterClient* m_client_player {};
	Character* m_team_mate {};
	std::function<void()> m_lose_callback;
};

