#pragma once

class LobbyState final : public State
{

	auto HandleTutorialPress() const;
	auto HandleTeamButtonPressed(sf::Int8 id);
	auto HandleStartGamePressed() const;
	auto IsHostAndInTeam();
	auto HandleLeaveTeamButtonPress();
	auto IsInATeam();
	auto HandleBackButtonPressed() const;


	void HandleTeamChoice(sf::Int8 id);
	void SendStartGameCountdown() const;
	void SendClientDisconnect(sf::Int8 id) const;
	void SendPlayerName(sf::Int8 id, const std::string& name) const;

	void CreateUI();

public:
	LobbyState();

	void Draw() override;
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void OnStackPopped() override;
private:
	GUI::Container m_gui_container;

	GUI::Button::Ptr m_change_name_button;
	GUI::Label::Ptr m_current_name_label;
	std::string m_player_input_name;
	GUI::Label::Ptr m_start_countdown_label;

	bool m_start_countdown;
	sf::Time m_start_countdown_timer;

	sf::Int8 m_player_id;
	std::map<sf::Int8, sf::Int8> m_player_team_selection;

	bool m_game_started;
	std::map<sf::Int8, std::vector<sf::Int8>> m_team_selections;

	bool m_connected;
	GUI::Container m_gui_fail_container;

	std::map<sf::Int8, GUI::Label::Ptr> m_players;
};
