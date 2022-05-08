class Server : public Engine
{
public:

	static bool StaticInit();

	void DoFrame() override;

	int Run() override;

	void HandleNewClient(ClientProxyPtr inClientProxy);
	void HandleLostClient(ClientProxyPtr inClientProxy);

	RoboCatPtr	GetCatForPlayer(int inPlayerId);
	void	SpawnCatForPlayer(int inPlayerId);


private:
	Server();

	bool	InitNetworkManager();
	void	SetupWorld();

};

