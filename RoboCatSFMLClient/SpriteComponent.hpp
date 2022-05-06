#pragma once
using TexturePtr = shared_ptr<sf::Texture>;
using FontPtr = shared_ptr<sf::Font>;

class SpriteComponent
{
public:
	explicit SpriteComponent(GameObject* inGameObject);
	~SpriteComponent();


	void SetTexture(TexturePtr inTexture);
	virtual sf::Sprite& GetSprite();


protected:
	sf::Sprite m_sprite;

	//don't want circular reference...
	GameObject* mGameObject;
};

using SpriteComponentPtr = shared_ptr<SpriteComponent>;
