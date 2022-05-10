#include "RoboCatPCH.hpp"
#include <SFML/Graphics/Shape.hpp>

GameObject::GameObject()
	: mColor(Colors::White),
	mCollisionRadius(1.f),
	mRotation(0.f),
	mScale(1.0f),
	mIndexInWorld(-1),
	mDoesWantToDie(false),
	mNetworkId(0)
{
}

void GameObject::Update()
{
	//object don't do anything by default...	
}


Vector3 GameObject::GetForwardVector()	const
{
	//should we cache this when you turn?
	//SDL appears to use radians while SFML uses degrees - make sure to convert to radians
	//before using!
	return Vector3(sinf(RoboMath::ToRadians(mRotation)), -cosf(RoboMath::ToRadians(mRotation)), 0.f);
}

void GameObject::SetNetworkId(int inNetworkId)
{
	//this doesn't put you in the map or remove you from it
	mNetworkId = inNetworkId;

}

unsigned GameObject::GetCategory() const
{
	return Category::Type::kNone;
}

sf::FloatRect GameObject::GetBoundingRect() const
{
	return sf::FloatRect{};
}

void GameObject::SetRotation(float inRotation)
{
	//should we normalize using fmodf?
	mRotation = inRotation;
}
