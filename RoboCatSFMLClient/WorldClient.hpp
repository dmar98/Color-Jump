#pragma once

class WorldClient final : public World
{
public:
	WorldClient();

	void LoadLevel() override;
	void Update() override;

	void SetCamera(sf::View camera);
	sf::View& GetCamera();

	Character* AddCharacterWithColor(int player_id, EColorType color, sf::IntRect rect, Vector3 spawn_pos) override;
	Character* AddCharacter(int player_id, EColorType color) override;
	Character* AddGhostCharacterWithColor(int identifier, EColorType color, const sf::IntRect& int_rect, const Vector3 spawn_pos) override;
	Character* AddGhostCharacter(int player_id, EColorType color) override;
	Character* GetClientCharacter() const;
	void SetTeammate(Character* character);
	Character* GetTeammate() const;

	void UpdateCharacterTransparencies(int team_id) const;
	void UpdateCharacterTransparencies() const;
	void RespawnClientCharacter() const;
	void SetCheckpointToPlatformWithID(int platform_id);
	Character* GetCharacter(int player_id) const;
	void SetPlatformOnCharacter(Character* character, int platform_id) const;
	void UpdatePlatform(int id, int platform_id, EPlatformType platform_type);

private:
	void CheckClientCollisions() const;
	void DestroyPlayerOutsideView() const;
	void OnReachedCheckpoint() const;
	void OnReachedGoal() const;
	void OnClientPlayerDeath() const;

	sf::View m_camera;
	Platform* m_checkpoint;
	CharacterClient* m_client_player;
	Character* m_team_mate {};
	std::function<void()> m_lose_callback;
};

