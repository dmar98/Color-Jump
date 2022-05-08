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
		NCS_Team_Change,
		NCS_Start_Network_Game,
		NCS_Team_Death,
		NCS_Checkpoint_Reached,
		NCS_Welcomed,
		NCS_State,
		NCS_Size
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

	float GetLastMoveProcessedByServerTimestamp() const
	{
		return mLastMoveProcessedByServerTimestamp;
	}

	void SetTeamID(int team_id);
	void SetName(const string& name);
	void SetPlayerColor(int i);

private:
	NetworkManagerClient();
	void Init(const SocketAddress& inServerAddress, const string& inName);

	void UpdateInfoPacket(NetworkClientState p_enum, const std::function<void()>& p_function);

	void UpdateSayingHello();
	void UpdateSendingQuit();
	void UpdateSendingPlatform();
	void UpdateSendingPlayer();
	void UpdateSendingGoalReached();
	void UpdateSendingTeamChange();
	void UpdateSendingStartGame();
	void UpdateSendingTeamDeath();
	void UpdateSendingCheckpoint();

	void SendHelloPacket();
	void SendQuitPacket();
	void SendPlatformPacket();
	void SendPlayerPacket();
	void SendGoalPacket();
	void SendTeamChangePacket();
	void SendStartGamePacket();
	void SendTeamDeathPacket();
	void SendCheckpointPacket();

	void HandleWelcomePacket(InputMemoryBitStream& inInputStream);
	void HandleStatePacket(InputMemoryBitStream& inInputStream);
	void ReadLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream);
	
	void HandleTeamChange(InputMemoryBitStream& inInputStream);

	void HandleGameObjectState(InputMemoryBitStream& inInputStream);
	static void HandleScoreBoardState(InputMemoryBitStream& inInputStream);

	void UpdateInfoUpdatePacket(NetworkClientState p_enum, const std::function<void()>& p_function);

	void UpdateSendingInputPacket();
	void UpdateSendingPosition();

	void SendInputPacket();
	void SendPositionPacket();

	void DestroyGameObjectsInMap(const IntToGameObjectMap& inObjectsToDestroy);

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
	int mColor;

	float mLastMoveProcessedByServerTimestamp{};

	WeightedTimedMovingAverage mAvgRoundTripTime;
	float mLastRoundTripTime;
};
