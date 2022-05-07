#pragma once
class KeyBinding
{
public:
	static bool StaticInit();
	static std::unique_ptr<KeyBinding> sInstance;
	sf::Keyboard::Key GetAssignedKey(PlayerAction action) const;
	void AssignKey(PlayerAction action, sf::Keyboard::Key key);

private:
	explicit KeyBinding();
	bool CheckAction(sf::Keyboard::Key key, PlayerAction& out) const;
	std::vector<PlayerAction> GetRealtimeActions() const;


	std::map<sf::Keyboard::Key, PlayerAction> m_key_map;
};

bool IsRealtimeAction(PlayerAction action);
