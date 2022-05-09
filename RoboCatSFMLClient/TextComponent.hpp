#pragma once
class TextComponent : DrawableComponent
{
public:
	TextComponent(GameObject* game_object);
	void InitText(const string& text, const sf::Font& font, unsigned font_size, sf::Color color);
	void SetText(const string& text);
	void SetFontSize(unsigned font_size);
	void SetColor(sf::Color color);
	void SetFont(const sf::Font& font);
	sf::Text& GetText();

	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Text m_text;
};

using TextComponentPtr = shared_ptr<TextComponent>;