#pragma once
class WorldServer : public World
{
public:
	WorldServer();

	void Update() override;
	void LoadLevel() override;
	Character* AddCharacterWithColor(sf::Int8 identifier, EColorType color, sf::IntRect rect, Vector3 spawn_pos) override;
	Character* AddCharacter(int identifier, int color, bool is_client_player) override;
	Character* AddGhostCharacterWithColor(const int identifier, EColorType color, const sf::IntRect& int_rect,
	                                      const Vector3 spawn_pos) override;
	Character* AddGhostCharacter(int identifier, int color) override;

private:

};

