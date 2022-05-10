#pragma once
enum class MissionStatus;

class Player
{
public:
	explicit Player(int player_id, const KeyBinding* binding = nullptr);
	void HandleEvent(const sf::Event& event, CommandQueue& commands);
	void HandleRealtimeInput(CommandQueue& commands);
	std::map<PlayerAction, Command>::mapped_type GetAction(PlayerAction action);
	
	bool IsLocal() const;

private:
	void InitialiseActions();

	std::map<PlayerAction, Command> m_action_binding{};
	const KeyBinding* m_key_binding;
	std::map<PlayerAction, bool> m_action_proxies;
	int m_player_id;
};
