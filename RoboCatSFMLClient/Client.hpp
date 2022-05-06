#pragma once
class Client final : public Engine
{
public:
	static bool StaticInit();

protected:
	Client();
	void RegisterStates() const;

	void DoFrame() override;
	void HandleEvent(sf::Event& p_event) override;
	bool PollEvent(sf::Event& p_event) override;

private:
	StateStack* m_stack;
};
