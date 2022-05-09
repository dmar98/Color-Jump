#pragma once

class RayGround : public GameObject
{
public:
	explicit RayGround(Character* character);
	sf::FloatRect GetBoundingRect() const override;
	unsigned GetCategory() const override;
	void SetFalling() const;

	Character* m_character;
};
