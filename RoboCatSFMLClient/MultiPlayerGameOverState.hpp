#pragma once

class MultiPlayerGameOverState final : public State
{
public:
	MultiPlayerGameOverState(bool has_won);
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void Draw() override;
	void HandleGameEnd(const int team_id, float completion_time);
	void Quit();

private:
	void UpdateLeaderboard() const;

private:
	sf::Sprite m_background_sprite;
	sf::Text m_title_text;
	GUI::Container m_gui_container;
	std::vector<GUI::Label*> m_leaderboard;
};

