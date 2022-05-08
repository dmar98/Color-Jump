class NetworkManagerServer : public NetworkManager
{
public:
	static NetworkManagerServer* sInstance;

	static bool StaticInit(uint16_t inPort);

	void ProcessPacket(InputMemoryBitStream& inInputStream,
	                   const SocketAddress& inFromAddress) override;
	void HandleConnectionReset(const SocketAddress& inFromAddress) override;

	void SendOutgoingPackets();
	void CheckForDisconnects();

	void RegisterGameObject(const GameObjectPtr& inGameObject);
	inline GameObjectPtr RegisterAndReturn(GameObject* inGameObject);
	void UnregisterGameObject(const GameObject* inGameObject);
	void SetStateDirty(int inNetworkId, uint32_t inDirtyState) const;

	void RespawnCats() const;

	ClientProxyPtr GetClientProxy(int inPlayerId) const;

private:
	NetworkManagerServer();

	void HandlePacketFromNewClient(InputMemoryBitStream& inInputStream,
	                               const SocketAddress& inFromAddress);
	void HandleTeamChange(const ClientProxyPtr& inClientProxy, InputMemoryBitStream& in_input_stream);
	auto NotifyOfTeamChange( const ClientProxyPtr& inClientProxy);
	void ProcessPacket(const ClientProxyPtr& inClientProxy, InputMemoryBitStream& inInputStream);

	void SendWelcomePacket(const ClientProxyPtr& inClientProxy);
	void SendPacketToAll(const std::function<void(const ClientProxyPtr& inClientProxy)>& p_function) const;
	void UpdateAllClients();

	static void AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream);
	static void AddScoreBoardStateToPacket(OutputMemoryBitStream& inOutputStream);

	void SendStatePacketToClient(const ClientProxyPtr& inClientProxy);
	static void WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream,
	                                          const ClientProxyPtr& inClientProxy);

	static void HandleInputPacket(const ClientProxyPtr& inClientProxy, InputMemoryBitStream& inInputStream);

	void HandleClientDisconnected(const ClientProxyPtr& inClientProxy);

	int GetNewNetworkId();

	using IntToClientMap = unordered_map<int, ClientProxyPtr>;
	using AddressToClientMap = unordered_map<SocketAddress, ClientProxyPtr>;

	AddressToClientMap mAddressToClientMap;
	IntToClientMap mPlayerIdToClientMap;

	int mNewPlayerId;
	int mNewNetworkId;
	
	float mClientDisconnectTimeout;
};


inline GameObjectPtr NetworkManagerServer::RegisterAndReturn(GameObject* inGameObject)
{
	GameObjectPtr toRet(inGameObject);
	RegisterGameObject(toRet);
	return toRet;
}
