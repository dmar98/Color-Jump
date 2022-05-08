#include "RoboCatServerPCH.hpp"

namespace
{
	constexpr float kRespawnDelay = 3.f;
}

ClientProxy::ClientProxy(const SocketAddress& inSocketAddress, string inName,
                         const int inPlayerId) :
	mDeliveryNotificationManager(false, true),
	mSocketAddress(inSocketAddress),
	mName(std::move(inName)),
	mPlayerId(inPlayerId),
	mTeamId(0),
	mColor(0),
	mTimeToRespawn(0.f),
	mIsLastMoveTimestampDirty(false)
{
	UpdateLastPacketTime();
}


void ClientProxy::UpdateLastPacketTime()
{
	mLastPacketFromClientTime = Timing::sInstance.GetTimef();
}

void ClientProxy::HandleCatDied()
{
	mTimeToRespawn = Timing::sInstance.GetFrameStartTime() + kRespawnDelay;
}

void ClientProxy::RespawnCatIfNecessary()
{
	if (mTimeToRespawn != 0.f && Timing::sInstance.GetFrameStartTime() > mTimeToRespawn)
	{
		dynamic_cast<Server*>(Engine::s_instance.get())->SpawnCatForPlayer(mPlayerId);
		mTimeToRespawn = 0.f;
	}
}

void ClientProxy::SetTeamID(const int team_id)
{
	mTeamId = team_id;
}

void ClientProxy::SetColor(const int color)
{
	mColor = color;
}
