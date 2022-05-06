#pragma once

class TutorialState final : public State
{
public:
	explicit TutorialState(StateStack& stack);

	void Draw();
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	GUI::Container m_gui_container{};
	sf::Sprite m_tutorial1;
	sf::Sprite m_tutorial2;
	sf::Sprite m_tutorial3;
};
