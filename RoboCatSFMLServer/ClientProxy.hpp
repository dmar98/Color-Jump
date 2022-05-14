#pragma once
class ClientProxy
{
public:
	ClientProxy(const SocketAddress& inSocketAddress, string inName, int inPlayerId);

	const SocketAddress& GetSocketAddress() const { return mSocketAddress; }
	int GetPlayerId() const { return mPlayerId; }
	sf::Vector2f GetPosition() const { return m_position; }
	const string& GetName() const { return mName; }
	int GetTeamID() const { return mTeamId; }
	bool GetReady() const { return mReady; }
	EColorType GetColor() const { return mColor; }

	void UpdateLastPacketTime();
	float GetLastPacketFromClientTime() const { return mLastPacketFromClientTime; }

	DeliveryNotificationManager& GetDeliveryNotificationManager()
	{
		return mDeliveryNotificationManager;
	}

	ReplicationManagerServer& GetReplicationManagerServer() { return mReplicationManagerServer; }

	void SetTeamID(int team_id);
	void SetColor(EColorType color);
	void SetName(const string& name);
	void SetIsReady(bool ready);
	void SetPosition(float x, float y);

	void SetIsLastMoveTimestampDirty(const bool inIsDirty)
	{
		mIsLastMoveTimestampDirty = inIsDirty;
	}

	bool IsLastMoveTimestampDirty() const { return mIsLastMoveTimestampDirty; }


private:
	DeliveryNotificationManager mDeliveryNotificationManager;
	ReplicationManagerServer mReplicationManagerServer;

	SocketAddress mSocketAddress;
	string mName;
	int mPlayerId;
	int mTeamId;
	EColorType mColor;
	bool mReady;

	float mLastPacketFromClientTime{};
	sf::Vector2f m_position;

	bool mIsLastMoveTimestampDirty;
};

using ClientProxyPtr = shared_ptr<ClientProxy>;
