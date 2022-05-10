//Changed by Dylan Goncalves Martins (D00242562)
#include "RoboCatClientPCH.hpp"

struct CharacterMover
{
	CharacterMover(const float vx, const float vy, const int player_id) : m_velocity(vx, vy), m_player_id(player_id)
	{
	}

	void operator()(CharacterClient& character) const
	{
		if (m_player_id == character.GetPlayerID())
			character.Accelerate(CharacterClient::GetMaxSpeed() * m_velocity);
	}

	sf::Vector2f m_velocity;
	int m_player_id;
};

struct CharacterJumpTrigger
{
	explicit CharacterJumpTrigger()
	= default;

	void operator()(CharacterClient& character) const
	{
		character.Jump();
	}
};

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize player one keys 
 */
void Player::InitialiseActions()
{
	m_action_binding[PlayerAction::kMoveLeft].action = DerivedAction<CharacterClient>(
		CharacterMover(-1, 0.f, m_player_id));
	m_action_binding[PlayerAction::kMoveRight].action = DerivedAction<CharacterClient>(
		CharacterMover(+1, 0.f, m_player_id));
	m_action_binding[PlayerAction::kMoveUp].action = DerivedAction<CharacterClient>(
		CharacterJumpTrigger());
}

std::map<PlayerAction, Command>::mapped_type Player::GetAction(const PlayerAction action)
{
	return m_action_binding[action];
}

bool Player::IsLocal() const
{
	// No key binding means this player is remote
	return m_key_binding != nullptr;
}

Player::Player(const int player_id, const KeyBinding* binding): m_key_binding(binding), m_player_id(player_id)
{
	InitialiseActions();

	for (auto& pair : m_action_binding)
		pair.second.category = Category::kPlayer;
}


void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		PlayerAction action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && !
			IsRealtimeAction(action))
		{
			commands.Push(GetAction(action));
		}
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	if (IsLocal())
	{
		const std::vector<PlayerAction> active_actions = m_key_binding->GetRealtimeActions();
		for (const PlayerAction action : active_actions)
			commands.Push(GetAction(action));
	}
}
