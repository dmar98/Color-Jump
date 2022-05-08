//Written by Dylan Goncalves Martins (D00242562)

#include "RoboCatClientPCH.hpp"

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize the ray with its character
 */
RayGround::RayGround(Character* character) : m_character(character)
{
}

//unsigned RayGround::GetCategory() const
//{
//	if (m_character->GetCategory() == Category::kPlayerOne)
//	{
//		return Category::kRayOne;
//	}
//
//	return Category::kRayTwo;
//}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Sets the ray to be 2 wide and 50 high
 */
sf::FloatRect RayGround::GetBoundingRect() const
{
	return {m_character->GetLocation().mX, m_character->GetLocation().mY, 2, 50};
}

void RayGround::SetFalling() const
{
	m_character->SetFalling();
}
