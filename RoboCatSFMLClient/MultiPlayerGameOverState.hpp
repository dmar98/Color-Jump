#pragma once

class MultiPlayerGameOverState final : public State
{
public:
	bool Update(float dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void Draw() override;
};
