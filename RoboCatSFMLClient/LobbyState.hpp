#pragma once

class LobbyState final : public State
{
	auto HandleTutorialPress() const
	{
		return [this]
		{
			RequestStackPush(StateID::kTutorial);
		};
	}

	auto HandleTeamButtonPressed(int id)
	{
		return [this, id]
		{
			HandleTeamChoice(id);
			Unready();
		};
	}

	auto HandleReadyPressed()
	{
		return [this]
		{
			HandleReady();
		};
	}

	auto IsValidTeamPresent() const
	{
		return [this]
		{
			return IsValidTeam();
		};
	}

	auto HandleLeaveTeamButtonPress()
	{
		return [this]
		{
			HandleTeamChoice(0);
			Unready();
		};
	}

	auto IsInATeam() const
	{
		return [this]
		{
			return IsInTeam();
		};
	}

	auto HandleBackButtonPressed()
	{
		return [this]
		{
			HandleBackButton();
		};
	}

	void Unready();
	void HandleReady();
	bool IsValidTeam() const;
	bool IsInTeam() const;
	void HandleBackButton();

	void HandleTeamChoice(int team_id);
	bool TeamHasPlace(int team_id);

	static sf::Vector2f GetUnpairedPos(int i);
	void Debug(const std::string& message) const;
	static void SendClientDisconnect();
	void SendPlayerName() const;

	void CreateUI();
	
	static sf::Vector2f GetTeamPos(int i);
	void UpdateTeamMembers(int team_id);
	void RemovePlayerFromTeam(int player_id);

public:
	LobbyState();

	void Draw() override;
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void OnStackPopped() override;

	void MovePlayer(int player_id, int team_id);
	void RemovePlayer(int player_id);
	void AddPlayer(int id, const std::string& label_text, bool ready);
	void MovePlayerBack(int player_id);
	void SetName(int player_id, const string& name);
	void Start();
	void SetReady(int player_id, bool ready);
	void StartCountDown();
	void HandleConnected();
	void Quit() const;

private:
	GUI::Container m_gui_container;

	GUI::Button::Ptr m_change_name_button;
	GUI::Label::Ptr m_current_name_label;
	GUI::Label::Ptr m_failed_connection_text;
	std::string m_player_input_name;
	GUI::Label::Ptr m_start_countdown_label;

	bool m_start_countdown;
	float m_start_countdown_timer;
	float m_failed_connection_timer;

	std::map<int, int> m_player_team_selection;

	bool m_game_started;
	std::map<int, std::vector<int>> m_team_selections;

	bool m_connected;
	GUI::Container m_gui_fail_container;

	std::map<int, GUI::Label::Ptr> m_players;

	bool m_ready;
	int m_local_id;
	bool m_is_connecting;
};
