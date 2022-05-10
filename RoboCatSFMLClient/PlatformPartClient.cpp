//Written by Paul Bichler (D00242563)
#include "RoboCatClientPCH.hpp"
#include "Platform.hpp"

//Written by Paul Bichler (D00242563)
PlatformPartClient::PlatformPartClient(const sf::IntRect sub_rect, PlatformClient* platform, const ETileType type)
	: PlatformPart(sub_rect, platform, type)
{
	m_sprite_component.reset(new SpriteComponent(this));
	SetTexture(TextureManager::sInstance->GetTexture("LevelTileSet"), sub_rect);
}

void PlatformPartClient::SetTexture(const TexturePtr texture, const sf::IntRect sub_rect) const
{
	m_sprite_component->SetTexture(texture, sub_rect);
}

sf::FloatRect PlatformPartClient::GetBoundingRect() const
{
	return m_sprite_component->getTransform().transformRect(m_sprite_component->GetSprite().getLocalBounds());
}
