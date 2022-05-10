#pragma once
#include "DrawableComponent.hpp"

using TexturePtr = shared_ptr<sf::Texture>;
using FontPtr = shared_ptr<sf::Font>;

class SpriteComponent : public DrawableComponent
{
public:
	explicit SpriteComponent(GameObject* inGameObject);

	void SetTexture(TexturePtr inTexture, sf::IntRect subRect = {});
	virtual sf::Sprite& GetSprite();
	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void DrawBoundingRect(sf::RenderTarget& target, sf::RenderStates states, const sf::FloatRect& bounding_rect) const;

protected:
	sf::Sprite m_sprite;
};

using SpriteComponentPtr = shared_ptr<SpriteComponent>;
