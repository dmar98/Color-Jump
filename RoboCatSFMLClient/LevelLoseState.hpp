#pragma once
class LevelLoseState final : public State
{
public:
	explicit LevelLoseState();
	void Draw() override;
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;


private:
	sf::Sprite m_background_sprite;
	sf::Text m_lost_text;
	GUI::Container m_gui_container{};
};

