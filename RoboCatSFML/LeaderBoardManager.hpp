#pragma once
class LeaderBoardManager
{
public:
	static void StaticInit();
	static std::unique_ptr<LeaderBoardManager> sInstance;

	std::vector<std::pair<int, float>>& GetLeaderboard();
	void AddToLeaderboard(int team_id, float completion_time);
	void SetLeaderboardChangeCallback(std::function<void()> callback);
	void AddPlayer(int team_id, int id, std::string name);
	std::vector<std::string> GetPlayerNamesOfTeam(int team_id);
	int GetNumberOfTeams() const;

private:
	LeaderBoardManager() = default;

	std::map<int, std::vector<sf::Int8>> m_teams;
	std::map<int, std::string> m_players;
	std::vector<std::pair<int, float>> m_leaderboard;
	std::function<void()> m_leaderboard_change_callback;
};

