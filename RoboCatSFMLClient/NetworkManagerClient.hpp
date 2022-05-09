#pragma once
class NetworkManagerClient final : public NetworkManager
{
public:
	enum class NetworkClientState
	{
		NCS_Uninitialized,
		NCS_Saying_Hello,
		NCS_Position_Update,
		NCS_Quit,
		NCS_Platform_Update,
		NCS_Player_Update,
		NCS_Goal_Reached,
		NCS_Start_Network_Game,
		NCS_Team_Death,
		NCS_Checkpoint_Reached,
		NCS_State,
		NCS_Size,

		NCS_Game_State,
	};

	static NetworkManagerClient* sInstance;

	static void StaticInit(const SocketAddress& inServerAddress, const string& inName);


	void SendOutgoingPackets();

	void ProcessPacket(InputMemoryBitStream& inInputStream,
	                   const SocketAddress& inFromAddress) override;

	const WeightedTimedMovingAverage& GetAvgRoundTripTime() const
	{
		return mAvgRoundTripTime;
	}

	float GetRoundTripTime() const
	{
		return mAvgRoundTripTime.GetValue();
	}

	int GetPlayerId() const
	{
		return mPlayerId;
	}

	void SetTeamID(int team_id);
	void SetName(const string& name);
	void SetPlayerColor(EColorType color);
	void SendReadyPacket();
	int GetTeamID() const { return mTeamID; }

	void SendReady(bool ready);
	void SendTeamChangePacket();
private:
	NetworkManagerClient();
	void Init(const SocketAddress& inServerAddress, const string& inName);
	static void HandleGameWon(InputMemoryBitStream& inInputStream);
	static void HandlePlatformPacket(InputMemoryBitStream& inInputStream);
	static void HandleTeamDeathPacket(InputMemoryBitStream& inInputStream);
	static void HandleCheckpointPacket(InputMemoryBitStream& inInputStream);
	static void ReadGhostData(InputMemoryBitStream& inInputStream);
	static void ReadPlayerData(InputMemoryBitStream& inInputStream);
	static void HandleSpawnPacket(InputMemoryBitStream& inInputStream);


	void UpdateSayingHello();
	void UpdateSendingQuit();
	void UpdateSendingPlatform();
	void UpdateSendingPlayer();
	void UpdateSendingGoalReached();
	void UpdateSendingStartGame();
	void UpdateSendingTeamDeath();
	void UpdateSendingCheckpoint();
	void UpdateSendingWaitState();

	void SendHelloPacket();
	void SendQuitPacket();
	void SendPlatformPacket();
	void SendPlayerPacket();
	void SendGoalPacket();
	
	void SendStartGamePacket();
	void SendTeamDeathPacket();
	void SendCheckpointPacket();
	void SendStatePacket();

	void HandleWelcomePacket(InputMemoryBitStream& inInputStream);
	void HandleStatePacket() const;
	void HandlePlayerPacket(InputMemoryBitStream& input_memory_bit_stream) const;
	static void HandleQuitPacket(InputMemoryBitStream& input_memory_bit_stream);
	void HandlePlayerNamePacket(InputMemoryBitStream& input_memory_bit_stream) const;
	void HandleInitialStatePacket(InputMemoryBitStream& input_memory_bit_stream);
	void HandleReadyChange(InputMemoryBitStream& input_memory_bit_stream);

	static void RemovePlayer(int player_id);
	static void AddPlayer(int player_id, const std::string& name, bool ready = false);
	static void MovePlayer(int player_id, int team_id);
	static void MovePlayerBack(int playerId);
	static void SetName(int player_id, const std::string& name);

	void HandleTeamChange(InputMemoryBitStream& inInputStream);
	void HandleStartPacket();
	static void HandleStartCountdownPacket();

	void UpdateInfoPacket(NetworkClientState p_enum, const std::function<void()>& p_function);
	void UpdateInfoUpdatePacket(NetworkClientState p_enum, const std::function<void()>& p_function);

	void UpdateSendingPosition();

	void SendInputPacket();
	void SendPositionPacket();

	DeliveryNotificationManager mDeliveryNotificationManager;

	SocketAddress mServerAddress;

	NetworkClientState mState = NetworkClientState::NCS_Uninitialized;
	


public:
	void SetState(NetworkClientState m_state);
private:
	std::map<NetworkClientState, float> mTimeOfLastPacket;

	string mName;
	int mPlayerId{};
	int mTeamID{};
	EColorType mColor;
	bool mReady{};

	WeightedTimedMovingAverage mAvgRoundTripTime;
};
