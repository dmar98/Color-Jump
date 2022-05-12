#pragma once
class NetworkManagerClient final : public NetworkManager
{
public:
	enum class NetworkClientState
	{
		NCS_Uninitialized,
		NCS_Saying_Hello,
		NCS_Quit,
		NCS_Name,
		NCS_Goal_Reached,
		NCS_Start_Network_Game,
		NCS_Team_Death,
		NCS_Team_Changed,
		NCS_Checkpoint_Reached,
		NCS_State,
		NCS_Game_State,
		NCS_Spawn,
		NCS_Ready,
		NCS_Platform,
		NCS_Color,
		NCS_Size,
	};

	static NetworkManagerClient* sInstance;

	static void StaticInit(const SocketAddress& inServerAddress, const string& inName);


	void SendOutgoingPackets();
	void ProcessPacket(InputMemoryBitStream& inInputStream,
	                   const SocketAddress& inFromAddress) override;

	void HandleConnectionReset(const SocketAddress& inFromAddress) override;

	const WeightedTimedMovingAverage& GetAvgRoundTripTime() const
	{
		return mAvgRoundTripTime;
	}

	float GetRoundTripTime() const
	{
		return mAvgRoundTripTime.GetValue();
	}

	int GetPlayerId() const { return mPlayerId; }
	int GetTeamID() const { return mTeamID; }
	EColorType GetColor() const { return mColor; }
	bool GetReady() const { return mReady; }
	int GetPlatformId() const { return mPlatformId; }
	EPlatformType GetPlatformType() const { return mPlatformType; }
	string GetName() const { return mName; }
	
	void SetState(NetworkClientState m_state);
	
	void UpdatePlayerPosition(float x, float y);
	void UpdateTeam(int team_id, EColorType color);
	void UpdateReady(bool ready);
	void UpdatePlatform(int platform_id, EPlatformType platform_type);
	void UpdateName(const string& name);
	void UpdateColor(EColorType color);

private:
	NetworkManagerClient();

	void Init(const SocketAddress& inServerAddress, const string& inName);

	static LobbyState* GetLobbyState();
	static MultiplayerGameState* GetMultiplayerGameState();


	void HandleGameWon(InputMemoryBitStream& inInputStream);
	void HandlePlatformPacket(InputMemoryBitStream& inInputStream);
	void HandleTeamDeathPacket(InputMemoryBitStream& inInputStream);
	void HandleCheckpointPacket(InputMemoryBitStream& inInputStream);
	void HandleSpawnPacket(InputMemoryBitStream& inInputStream);
	void HandleWelcomePacket(InputMemoryBitStream& inInputStream);


	void UpdatePositions();
	void UpdateTeamChanged();
	void UpdateColor();
	void UpdateSayingHello();
	void UpdateReady();
	void UpdatePlatform();
	void UpdateSendingQuit();
	void UpdateSendingPlayer();
	void UpdateSendingGoalReached();
	void UpdateSendingStartGame();
	void UpdateSendingTeamDeath();
	void UpdateSendingCheckpoint();
	void UpdateSendingWaitState();

	void SendHelloPacket();
	void SendQuitPacket();
	void SendPlayerPacket();
	void SendGoalPacket();
	void SendStartGamePacket();
	void SendTeamDeathPacket();
	void SendCheckpointPacket();
	void SendStatePacket();
	void SendPositionPacket();
	void SendReadyPacket();
	void SendPlatformPacket();
	void SendTeamPacket();
	void SendColorPacket();

	void HandleStatePacket() const;
	void HandleGameStatePacket(InputMemoryBitStream& inInputStream) const;
	void HandlePlayerPacket(InputMemoryBitStream& input_memory_bit_stream) const;

	void HandleQuitPacket(InputMemoryBitStream& input_memory_bit_stream);
	void HandlePlayerNamePacket(InputMemoryBitStream& input_memory_bit_stream);
	void HandleInitialStatePacket(InputMemoryBitStream& input_memory_bit_stream) const;
	void HandleReadyChange(InputMemoryBitStream& input_memory_bit_stream);

	void HandleTeamChange(InputMemoryBitStream& inInputStream);
	void HandleColorChange(InputMemoryBitStream& inInputStream);
	void HandleStartPacket();
	static void HandleStartCountdownPacket();

	void UpdateInfoPacket(NetworkClientState p_enum, const std::function<void()>& p_function);
	void UpdateInfoUpdatePacket(NetworkClientState p_enum, const std::function<void()>& p_function);

	void SendInputPacket();

	static void ReadGhostData(InputMemoryBitStream& inInputStream);
	static void ReadPlayerData(InputMemoryBitStream& inInputStream);


	DeliveryNotificationManager mDeliveryNotificationManager;
	SocketAddress mServerAddress;
	NetworkClientState mState;
	std::map<NetworkClientState, float> mTimeOfLastPacket;

	string mName;
	int mPlayerId;
	int mTeamID;
	int mPlatformId;
	EColorType mColor;
	bool mReady;
	float m_x;
	float m_y;
	EPlatformType mPlatformType;

	WeightedTimedMovingAverage mAvgRoundTripTime;

	void Debug(const std::string& message) const
	{
		const string in_format = "Network Client: " + message;
		LOG(in_format.c_str(), 0)
	}
};
