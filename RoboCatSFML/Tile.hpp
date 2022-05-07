#pragma once
#include "EColorType.hpp"
#include "ETileType.hpp"


class Tile : public GameObject
{
public:
	explicit Tile(sf::IntRect sub_rect, ETileType type);

	ETileType GetType() const;
	void SetActiveCollider(bool active);
	virtual sf::FloatRect GetBoundingRect() const;
	virtual bool HandleCollision(EColorType color);
	virtual sf::IntRect GetSize() const;

protected:
	ETileType m_type;
	bool m_has_collider;
	sf::IntRect m_size;
};

