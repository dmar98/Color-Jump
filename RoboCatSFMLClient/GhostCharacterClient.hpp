#pragma once
class GhostCharacterClient : public CharacterClient
{
public:
	GhostCharacterClient(EColorType type, const sf::IntRect& texture_rect);
	void Update() override;
	sf::FloatRect GetBoundingRect() const override;
};

