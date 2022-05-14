#pragma once
class Server final : public Engine
{
public:
	static bool StaticInit();

	void DoFrame() override;

	int Run() override;

private:
	Server();

	bool InitNetworkManager() const;
	void SetupWorld();
};
