#include "ColorJumpClientPCH.hpp"

std::unique_ptr<KeyBinding> KeyBinding::sInstance;

bool KeyBinding::StaticInit()
{
	sInstance.reset(new KeyBinding());
	return true;
}

KeyBinding::KeyBinding()
{
	m_key_map[sf::Keyboard::A] = PlayerAction::kMoveLeft;
	m_key_map[sf::Keyboard::D] = PlayerAction::kMoveRight;
	m_key_map[sf::Keyboard::W] = PlayerAction::kMoveUp;
}

void KeyBinding::AssignKey(const PlayerAction action, const sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = m_key_map.begin(); itr != m_key_map.end();)
	{
		if (itr->second == action)
			m_key_map.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	m_key_map[key] = action;
}

sf::Keyboard::Key KeyBinding::GetAssignedKey(const PlayerAction action) const
{
	for (const auto& pair : m_key_map)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

bool KeyBinding::CheckAction(const sf::Keyboard::Key key, PlayerAction& out) const
{
	const auto found = m_key_map.find(key);
	if (found == m_key_map.end())
	{
		return false;
	}
	out = found->second;
	return true;
}

std::vector<PlayerAction> KeyBinding::GetRealtimeActions() const
{
	// Return all realtime actions that are currently active.
	std::vector<PlayerAction> actions;

	for (auto& pair : m_key_map)
	{
		// If key is pressed and an action is a realtime action, store it
		if (sf::Keyboard::isKeyPressed(pair.first) && IsRealtimeAction(pair.second))
			actions.push_back(pair.second);
	}

	return actions;
}

bool IsRealtimeAction(const PlayerAction action)
{
	switch (action)
	{
	case PlayerAction::kMoveLeft:
	case PlayerAction::kMoveRight:
		return true;
	case PlayerAction::kMoveUp:
	case PlayerAction::kActionCount:
	default:
		return false;
	}
}
