#pragma once
class Server final : public Engine
{
public:
	static bool StaticInit();

	void DoFrame() override;

	int Run() override;

	static void HandleNewClient(const ClientProxyPtr& inClientProxy);
	static void HandleLostClient(const ClientProxyPtr& inClientProxy);

private:
	Server();

	bool InitNetworkManager() const;
	void SetupWorld();
};
