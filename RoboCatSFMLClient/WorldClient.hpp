#pragma once

class WorldClient final : public World
{
public:
	WorldClient();

	void LoadLevel() override;
	void Update() override;

	void SetCamera(sf::View camera);
	sf::View& GetCamera();

	Character* AddCharacter(int player_id, EColorType color) override;
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
	CommandQueue& GetCommandQueue();
	void Debug(const std::string& message) const;
	void RemoveCharacter(int player_id);

private:
	void CheckClientCollisions() const;
	void DestroyPlayerOutsideView() const;
	void OnReachedCheckpoint() const;
	static void OnReachedGoal();
	void OnClientPlayerDeath() const;

	sf::View m_camera;
	Platform* m_checkpoint;
	CharacterClient* m_client_player{};
	Character* m_team_mate {};
	std::function<void()> m_lose_callback;
	CommandQueue m_command_queue;
};

