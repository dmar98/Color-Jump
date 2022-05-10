#pragma once
class WorldServer final : public World
{
public:
	~WorldServer() override;
	void Update() override;
	void LoadLevel() override;
	Character* AddCharacterWithColor(int player_id, EColorType color, sf::IntRect rect,
		Vector3 spawn_pos) override;
	Character* AddCharacter(int identifier, EColorType color) override;
	Character* AddGhostCharacterWithColor(const int identifier, EColorType color,
		const sf::IntRect& int_rect, const Vector3 spawn_pos) override;
	Character* AddGhostCharacter(const int identifier, const EColorType color) override;
protected:
	void HandleCollisions() override;
public:
	WorldServer();
};

