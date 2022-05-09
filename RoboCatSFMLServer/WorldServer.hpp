#pragma once
class WorldServer : public World
{
public:
	WorldServer();

	void Update() override;
	void LoadLevel() override;
	Character* AddCharacterWithColor(sf::Int8 identifier, EColorType color, sf::IntRect rect, Vector3 spawn_pos) override;
	Character* AddCharacter(sf::Int8 identifier, sf::Int8 color, bool is_client_player) override;
	Character* AddGhostCharacterWithColor(sf::Int8 identifier, EColorType color, const sf::IntRect& int_rect,
		Vector3 spawn_pos) override;
	Character* AddGhostCharacter(sf::Int8 identifier, sf::Int8 color) override;

private:

};

