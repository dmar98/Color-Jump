#pragma once
class LeaderBoardManager
{
public:
	static void StaticInit();
	static std::unique_ptr<LeaderBoardManager> sInstance;

	std::vector<std::pair<sf::Int8, sf::Time>>& GetLeaderboard();
	void AddToLeaderboard(sf::Int8 team_id, sf::Time completion_time);
	void SetLeaderboardChangeCallback(std::function<void()> callback);
	void AddPlayer(sf::Int8 team_id, sf::Int8 id, std::string name);
	std::vector<std::string> GetPlayerNamesOfTeam(sf::Int8 team_id);
	int GetNumberOfTeams() const;

private:
	LeaderBoardManager() = default;

	std::map<sf::Int8, std::vector<sf::Int8>> m_teams;
	std::map<sf::Int8, std::string> m_players;
	std::vector<std::pair<sf::Int8, sf::Time>> m_leaderboard;
	std::function<void()> m_leaderboard_change_callback;
};

