class ClientProxy
{
public:
	ClientProxy(const SocketAddress& inSocketAddress, string inName, int inPlayerId);

	const SocketAddress& GetSocketAddress() const { return mSocketAddress; }
	int GetPlayerId() const { return mPlayerId; }
	const string& GetName() const { return mName; }
	int GetTeamID() const { return mTeamId; }
	int GetColor() const { return mColor; }
	void SetInputState(const InputState& inInputState) { mInputState = inInputState; }
	const InputState& GetInputState() const { return mInputState; }

	void UpdateLastPacketTime();
	float GetLastPacketFromClientTime() const { return mLastPacketFromClientTime; }

	DeliveryNotificationManager& GetDeliveryNotificationManager()
	{
		return mDeliveryNotificationManager;
	}

	ReplicationManagerServer& GetReplicationManagerServer() { return mReplicationManagerServer; }

	const MoveList& GetUnprocessedMoveList() const { return mUnprocessedMoveList; }
	MoveList& GetUnprocessedMoveList() { return mUnprocessedMoveList; }

	void SetIsLastMoveTimestampDirty(const bool inIsDirty)
	{
		mIsLastMoveTimestampDirty = inIsDirty;
	}

	bool IsLastMoveTimestampDirty() const { return mIsLastMoveTimestampDirty; }

	void HandleCatDied();
	void RespawnCatIfNecessary();
	void SetTeamID(int team_id);
	void SetColor(int color);


private:
	DeliveryNotificationManager mDeliveryNotificationManager;
	ReplicationManagerServer mReplicationManagerServer;

	SocketAddress mSocketAddress;
	string mName;
	int mPlayerId;
	int mTeamId;
	int mColor;

	InputState mInputState;

	float mLastPacketFromClientTime{};
	float mTimeToRespawn;

	MoveList mUnprocessedMoveList;
	bool mIsLastMoveTimestampDirty;
};

using ClientProxyPtr = shared_ptr<ClientProxy>;
