#pragma once

class RayGround : public GameObject
{
public:
	explicit RayGround(Character* character);
	sf::FloatRect GetBoundingRect() const;
	void SetFalling() const;
	Character* m_character;
};
