#pragma once
#include "WorldClient.hpp"

class MultiplayerGameState final : public State
{
public:
	MultiplayerGameState();

	void OnStackPopped() override;

	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void Draw() override;

private:
	WorldClient* m_world_client;
};
