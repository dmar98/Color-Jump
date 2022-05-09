#include "RoboCatClientPCH.hpp"

GhostCharacterClient::GhostCharacterClient(EColorType type, const sf::IntRect& texture_rect)
	: CharacterClient(type, texture_rect)
{

}

void GhostCharacterClient::Update()
{
	//Ghost characters do nothing locally!
}

sf::FloatRect GhostCharacterClient::GetBoundingRect() const
{
	//Collisions of Ghost characters are not handled!
	return {0,0,0,0};
}

unsigned GhostCharacterClient::GetCategory() const
{
	return Category::Type::kGhost;
}
