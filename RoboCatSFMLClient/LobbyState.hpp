#pragma once

class LobbyState final : public State
{

	auto HandleTutorialPress() const;
	auto HandleTeamButtonPressed(int id);
	auto HandleStartGamePressed() const;
	auto IsHostAndInTeam();
	auto HandleLeaveTeamButtonPress();
	auto IsInATeam();
	auto HandleBackButtonPressed() const;
	std::map<int, std::vector<int>>::mapped_type GetPlayerTeam(int player_id);


	void HandleTeamChoice(int team_id);
	
	static sf::Vector2f GetUnpairedPos(int i);
	static void SendClientDisconnect();
	void SendPlayerName() const;

	void CreateUI();

public:
	LobbyState();

	void Draw() override;
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void OnStackPopped() override;
	void MovePlayer(int player_id, int team_id);
	void AddPlayer(int id, const std::string& label_text);
	void MovePlayerBack(int id);
	static sf::Vector2f GetTeamPos(int i);
private:
	GUI::Container m_gui_container;

	GUI::Button::Ptr m_change_name_button;
	GUI::Label::Ptr m_current_name_label;
	std::string m_player_input_name;
	GUI::Label::Ptr m_start_countdown_label;

	bool m_start_countdown;
	sf::Time m_start_countdown_timer;

	int m_player_id;
	std::map<int, int> m_player_team_selection;

	bool m_game_started;
	std::map<int, std::vector<int>> m_team_selections;

	bool m_connected;
	GUI::Container m_gui_fail_container;

	std::map<int, GUI::Label::Ptr> m_players;
	int m_unpaired_y_pos;
};

