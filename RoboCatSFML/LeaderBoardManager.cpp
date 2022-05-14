#include "ColorJumpPCH.hpp"

std::unique_ptr<LeaderBoardManager>	LeaderBoardManager::sInstance;

void LeaderBoardManager::StaticInit()
{
	sInstance.reset(new LeaderBoardManager());
}

//Written by Paul Bichler (D00242563)
std::vector<std::pair<int, float>>& LeaderBoardManager::GetLeaderboard()
{
	return m_leaderboard;
}

//Written by Paul Bichler (D00242563)
//Adds a team to the leaderboard (called when a team finishes the level)
void LeaderBoardManager::AddToLeaderboard(int team_id, float completion_time)
{
	for (const auto& team : m_leaderboard)
		if (team.first == team_id)
			return;

	m_leaderboard.emplace_back(std::pair<int, float>(team_id, completion_time));

	if (m_leaderboard_change_callback)
		m_leaderboard_change_callback();
}

//Written by Paul Bichler (D00242563)
//this callback is invoked when the leaderboard changes (a team is added to the leaderboard for example)
//This is used to update the leaderboard for clients that already finished the game and are in the win/lose state
void LeaderBoardManager::SetLeaderboardChangeCallback(std::function<void()> callback)
{
	m_leaderboard_change_callback = std::move(callback);
}

//Written by Paul Bichler (D00242563)
//Adds a player to a team with its name
void LeaderBoardManager::AddPlayer(int team_id, int id, std::string name)
{
	m_players.try_emplace(id, name);
	m_teams[team_id].emplace_back(id);
}

//Written by Paul Bichler (D00242563)
//Returns the names of the two players a team
std::vector<std::string> LeaderBoardManager::GetPlayerNamesOfTeam(int team_id)
{
	std::vector<std::string> player_names;
	const std::vector<sf::Int8> team = m_teams[team_id];

	player_names.reserve(team.size());
	for (auto player_id : team)
	{
		player_names.emplace_back(m_players[player_id]);
	}

	return player_names;
}

//Written by Paul Bichler (D00242563)
int LeaderBoardManager::GetNumberOfTeams() const
{
	return m_teams.size();
}
