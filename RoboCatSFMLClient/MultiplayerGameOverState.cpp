#include "RoboCatClientPCH.hpp"

MultiPlayerGameOverState::MultiPlayerGameOverState(bool has_won)
{
	const sf::Font& font = *FontManager::sInstance->GetFont("carlito");
	const sf::Vector2f viewSize = WindowManager::sInstance->getView().getSize();

	//Create the Win/Lose Title Text
	if (has_won)
	{
		m_title_text.setString("You Win!");
		m_title_text.setFillColor(sf::Color::Green);
	}
	else
	{
		m_title_text.setString("You Lose!");
		m_title_text.setFillColor(sf::Color::Red);
	}

	m_title_text.setFont(font);
	m_title_text.setCharacterSize(70);
	Utility::CentreOrigin(m_title_text);
	m_title_text.setPosition(0.5f * viewSize.x, 0.2f * viewSize.y);

	//Create the leaderboard labels
	LeaderBoardManager::sInstance->SetLeaderboardChangeCallback([this] { UpdateLeaderboard(); });
	const int number_of_teams = LeaderBoardManager::sInstance->GetNumberOfTeams();
	for (int i = 0; i < number_of_teams; i++)
	{
		const auto label = std::make_shared<GUI::Label>("", 25.f);
		label->setPosition(250.f, 300.f + i * 35.f);
		m_leaderboard.emplace_back(label.get());
		m_gui_container.Pack(label);
	}
	UpdateLeaderboard();

	//Create the Back to Main Menu button
	auto main_menu_button = std::make_shared<GUI::Button>();
	Utility::CreateButton(main_menu_button, 0.5f * viewSize.x - 100, 0.8f * viewSize.y, "Back to Main Menu", [this]()
		{
			NetworkManagerClient::sInstance->SetState(NetworkManagerClient::NetworkClientState::NCS_Quit);
			RequestStackClear();
			RequestStackPush(StateID::kMenu);
		});
	m_gui_container.Pack(main_menu_button);
}

bool MultiPlayerGameOverState::Update(float dt)
{
	NetworkManagerClient::sInstance->ProcessIncomingPackets();

	//This state is pushed onto the MultiplayerGameState, which needs to update as well,
	//otherwise the client will be timed out
	return true;
}

bool MultiPlayerGameOverState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}

void MultiPlayerGameOverState::Draw()
{
	sf::RenderWindow& window = *WindowManager::sInstance;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(m_title_text);
	window.draw(m_gui_container);
}

void MultiPlayerGameOverState::HandleGameEnd(const int team_id, float completion_time)
{
	LeaderBoardManager::sInstance->AddToLeaderboard(team_id, completion_time);
}

void MultiPlayerGameOverState::Quit()
{
	RequestStackClear();
	RequestStackPush(StateID::kMenu);
}

void MultiPlayerGameOverState::UpdateLeaderboard() const
{
	const auto leaderboard = LeaderBoardManager::sInstance->GetLeaderboard();

	//Go through all of the players in the leaderboard (stored in the context) and update the labels
	for (int i = 0; i < leaderboard.size(); i++)
	{
		auto player_names = LeaderBoardManager::sInstance->GetPlayerNamesOfTeam(leaderboard[i].first);
		std::string team_id = std::to_string(leaderboard[i].first);
		std::string completion_time = std::to_string(leaderboard[i].second);
		std::string text = std::to_string(i + 1) + ".\tTeam " + team_id + "\t(" + player_names[0] + " & " + player_names[1] + ")\tTime: " + completion_time + " s";
		m_leaderboard[i]->SetText(text);
	}
}
