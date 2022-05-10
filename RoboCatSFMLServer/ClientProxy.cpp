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
	mColor(EColorType::kBlue),
	mTimeToRespawn(0.f)
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

void ClientProxy::SetColor(const EColorType color)
{
	mColor = color;
}

void ClientProxy::SetName(const string& name)
{
	mName = name;
}

void ClientProxy::SetIsReady(const bool ready)
{
	mReady = ready;
}

void ClientProxy::SetPosition(const float x, const float y)
{
	m_position = sf::Vector2f(x, y);
}
