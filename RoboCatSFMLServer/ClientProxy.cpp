#include "RoboCatServerPCH.hpp"

ClientProxy::ClientProxy(const SocketAddress& inSocketAddress, string inName,
                         const int inPlayerId) :
	mDeliveryNotificationManager(false, true),
	mSocketAddress(inSocketAddress),
	mName(std::move(inName)),
	mPlayerId(inPlayerId),
	mTeamId(0),
	mColor(EColorType::kBlue)
{
	UpdateLastPacketTime();
}


void ClientProxy::UpdateLastPacketTime()
{
	mLastPacketFromClientTime = Timing::sInstance.GetTimef();
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
