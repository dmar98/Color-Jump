#include "RoboCatClientPCH.hpp"

DrawableComponent::DrawableComponent(GameObject* game_object)
	: m_game_object(game_object),
	  m_draw_predicate([] { return true; })
{
	//and add yourself to the rendermanager...
	RenderManager::sInstance->AddComponent(this);
}

DrawableComponent::~DrawableComponent()
{
	//don't render me, I'm dead!
	RenderManager::sInstance->RemoveComponent(this);
}

void DrawableComponent::UpdatePosition()
{
	const auto pos = m_game_object->GetLocation();
	const auto rot = m_game_object->GetRotation();
	setPosition(pos.mX + m_offset.x, pos.mY + m_offset.y);
	setRotation(rot);
}

void DrawableComponent::SetDrawPredicate(const std::function<bool()>& predicate)
{
	m_draw_predicate = predicate;
}

void DrawableComponent::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!m_draw_predicate())
		return;

	states.transform *= getTransform();
	DrawCurrent(target, states);
}

void DrawableComponent::SetOffset(const sf::Vector2f& offset)
{
	m_offset = offset;
}
