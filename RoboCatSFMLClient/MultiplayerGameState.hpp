#pragma once

class MultiplayerGameState final : public State
{
public:
	MultiplayerGameState();
	~MultiplayerGameState() override;

	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void Draw() override;
};
