//Written by Dylan Goncalves Martins (D00242562)

#include "RoboCatClientPCH.hpp"

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize the ray with its character
 */
RayGround::RayGround(Character* character) : m_character(character)
{
	
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Sets the ray to be 2 wide and 50 high
 */
sf::FloatRect RayGround::GetBoundingRect() const
{
	return {GetLocation().mX, GetLocation().mY, 2, 50};
}

unsigned RayGround::GetCategory() const
{
	return Category::Type::kRay;
}

void RayGround::SetFalling() const
{
	m_character->SetFalling();
}
