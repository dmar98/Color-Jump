#pragma once

class LevelPauseState final : public State
{
public:
	explicit LevelPauseState();

	void Draw() override;
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;


private:
	sf::Sprite m_background_sprite;
	sf::Text m_paused_text;
	GUI::Container m_gui_container{};
};
