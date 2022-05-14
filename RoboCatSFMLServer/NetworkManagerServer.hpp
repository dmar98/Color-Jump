#pragma once
class NetworkManagerServer final : public NetworkManager
{
public:
	static NetworkManagerServer* sInstance;

	static bool StaticInit(uint16_t inPort);

	void ProcessPacket(InputMemoryBitStream& inInputStream,
	                   const SocketAddress& inFromAddress) override;
	void HandleConnectionReset(const SocketAddress& inFromAddress) override;
	void CheckForDisconnects();

	void RegisterGameObject(const GameObjectPtr& inGameObject);
	inline GameObjectPtr RegisterAndReturn(GameObject* inGameObject);
	void UnregisterGameObject(const GameObject* inGameObject);
	void SetStateDirty(int inNetworkId, uint32_t inDirtyState) const;

	ClientProxyPtr GetClientProxy(int inPlayerId) const;

	void Update(float dt);
	void NotifyGameStart(const ClientProxyPtr& client);
	void NotifySpawnCharacters();

private:
	NetworkManagerServer();

	void HandlePacketFromNewClient(InputMemoryBitStream& inInputStream,
	                               const SocketAddress& inFromAddress);
	void HandleTeamChange(const ClientProxyPtr& inClientProxy,
	                      InputMemoryBitStream& inInputStream);
	void HandleColorChange(const ClientProxyPtr& inClientProxy,
	                       InputMemoryBitStream& inInputStream);

	void HandlePlayerNameChange(const ClientProxyPtr& client_proxy,
	                            InputMemoryBitStream& inInputStream);
	void HandleGoalReached(InputMemoryBitStream& inInputStream);
	void HandleTeamDeath(InputMemoryBitStream& inInputStream);
	void HandleCheckpoint(InputMemoryBitStream& inInputStream);
	void HandlePlatformChange(InputMemoryBitStream& inInputStream);
	void NotifyCountDown(const ClientProxyPtr& client);
	void CheckIfAllReady();
	void HandleReadyPacket(const ClientProxyPtr& client_proxy, InputMemoryBitStream& inInputStream);
	void HandleGameStatePacket(const ClientProxyPtr& client_proxy, InputMemoryBitStream& inInputStream);
	void ProcessPacket(const ClientProxyPtr& inClientProxy, InputMemoryBitStream& inInputStream);

	void SendInitialState(const ClientProxyPtr& client_proxy);
	void SendWelcomePacket(const ClientProxyPtr& inClientProxy);
	void NotifyPlayerJoin(const ClientProxyPtr& inClientProxy, int player_id, const string&
	                      name);
	void SendPacketToAll(
		const std::function<void(const ClientProxyPtr& inClientProxy)>& p_function) const;

	static void AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream);

	void SendStatePacketToClient(const ClientProxyPtr& inClientProxy);
	void SendGameStatePacket(const ClientProxyPtr& inClientProxy, InputMemoryBitStream& inInputStream);
	void HandleClientDisconnected(const ClientProxyPtr& inClientProxy);
	void NotifyPlayerQuit(const ClientProxyPtr& client, int player_id);

	int GetNewNetworkId();


	void NotifyOfTeamChange(const ClientProxyPtr& inClientProxy, int player_id, int team_id);
	void NotifyPlayerNameChange(const ClientProxyPtr& inClientProxy, int player_id, const string&
	                            name);
	void NotifyGoalReached(const ClientProxyPtr& inClientProxy, int player_id, int team_id);
	void NotifyTeamRespawn(const ClientProxyPtr& inClientProxy, int player_id, int team_id);
	void NotifyCheckpointReached(const ClientProxyPtr& inClientProxy, int player_id, int team_id, int platform_id);
	void NotifyPlatformUpdate(const ClientProxyPtr& inClientProxy, int player_id, int platform_id,
	                          EPlatformType platform_color);
	void NotifyReadyChange(const ClientProxyPtr& inClientProxy, int player_id, bool ready);

	using IntToClientMap = unordered_map<int, ClientProxyPtr>;
	using AddressToClientMap = unordered_map<SocketAddress, ClientProxyPtr>;

	AddressToClientMap mAddressToClientMap;
	IntToClientMap mPlayerIdToClientMap;

	int mNewPlayerId;
	int mNewNetworkId;

	float mClientDisconnectTimeout;
	bool m_start_countdown;
	float m_start_countdown_timer;
	bool m_game_started;
	float m_game_time;
};


inline GameObjectPtr NetworkManagerServer::RegisterAndReturn(GameObject* inGameObject)
{
	GameObjectPtr toRet(inGameObject);
	RegisterGameObject(toRet);
	return toRet;
}
