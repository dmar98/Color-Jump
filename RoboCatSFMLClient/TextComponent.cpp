#include "RoboCatClientPCH.hpp"

TextComponent::TextComponent(GameObject* game_object)
	: DrawableComponent(game_object)
{
}

void TextComponent::InitText(const string& text, const sf::Font& font, unsigned font_size, sf::Color color)
{
	SetText(text);
	SetColor(color);
	SetFontSize(font_size);
	SetFont(font);
}

void TextComponent::SetText(const string& text)
{
	m_text.setString(text);
	/*Utility::CentreOrigin(m_text);*/
	const sf::FloatRect bounds = m_text.getLocalBounds();
	m_text.setOrigin(std::floor(bounds.left - bounds.width / 2.f), std::floor(bounds.top - bounds.height / 2.f));
	setOrigin(m_text.getOrigin());
	m_text.setScale(sf::Vector2f(m_game_object->GetScale(), m_game_object->GetScale()));
}

void TextComponent::SetFontSize(unsigned font_size)
{
	m_text.setCharacterSize(font_size);
}

void TextComponent::SetColor(sf::Color color)
{
	m_text.setFillColor(color);
}

void TextComponent::SetFont(const sf::Font& font)
{
	m_text.setFont(font);
}

sf::Text& TextComponent::GetText()
{
	UpdatePosition();
	return m_text;
}

void TextComponent::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_text, states);
}
