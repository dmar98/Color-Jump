//Written by Paul Bichler (D00242563)
#include "ColorJumpServerPCH.hpp"

//Written by Paul Bichler (D00242563)
PlatformPartServer::PlatformPartServer(const sf::IntRect sub_rect, Platform* platform, const ETileType type)
	: PlatformPart(sub_rect, platform, type)
{
}

//Written by Paul Bichler (D00242563)
sf::FloatRect PlatformPartServer::GetBoundingRect() const
{
	/*return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());*/
	return {};
}
