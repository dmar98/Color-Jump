#include "RoboCatClientPCH.hpp"

NetworkManagerClient* NetworkManagerClient::sInstance;

namespace
{
	constexpr float kTimeBetweenPackets = 1.f;
	constexpr float kTimeBetweenUpdatePackets = 0.033f;
}

void NetworkManagerClient::SetTeamID(const int team_id)
{
	mTeamID = team_id;
}

void NetworkManagerClient::SetName(const string& name)
{
	mName = name;
}

void NetworkManagerClient::SetPlayerColor(int i)
{
	mColor = i;
}

NetworkManagerClient::NetworkManagerClient() :
	mDeliveryNotificationManager(true, false),
	mLastRoundTripTime(0.f)
{
}

void NetworkManagerClient::StaticInit(const SocketAddress& inServerAddress, const string& inName)
{
	sInstance = new NetworkManagerClient();
	return sInstance->Init(inServerAddress, inName);
}

void NetworkManagerClient::Init(const SocketAddress& inServerAddress, const string& inName)
{
	NetworkManager::Init(0);

	mServerAddress = inServerAddress;
	mState = NetworkClientState::NCS_Saying_Hello;
	mName = inName;

	for (int i = 0; i < static_cast<int>(NetworkClientState::NCS_Size); ++i)
	{
		mTimeOfLastPacket.try_emplace(static_cast<NetworkClientState>(i), 0);
	}

	mAvgRoundTripTime = WeightedTimedMovingAverage(1.f);
}

void NetworkManagerClient::ProcessPacket(InputMemoryBitStream& inInputStream,
                                         const SocketAddress& inFromAddress)
{
	PacketType packetType;
	inInputStream.Read(packetType);
	switch (packetType)
	{
	case PacketType::PT_Welcomed:
		HandleWelcomePacket(inInputStream);
		break;
	case PacketType::PT_State: break;
	case PacketType::PT_Hello: break;
	case PacketType::PT_Quit: break;
	case PacketType::PT_Platform: break;
	case PacketType::PT_Player: break;
	case PacketType::PT_Goal: break;
	case PacketType::PT_Team_Changed:
		HandleTeamChange(inInputStream);
		break;
	case PacketType::PT_Start_Game: break;
	case PacketType::PT_Start_Game_Countdown: break;
	case PacketType::PT_Team_Death: break;
	case PacketType::PT_Checkpoint: break;
	default:
		break;
	}
}


void NetworkManagerClient::SendOutgoingPackets()
{
	switch (mState)
	{
	case NetworkClientState::NCS_Saying_Hello:
		UpdateSayingHello();
		break;
	case NetworkClientState::NCS_Welcomed:
		UpdateSendingInputPacket();
		break;
	case NetworkClientState::NCS_Position_Update:
		UpdateSendingPosition();
		break;
	case NetworkClientState::NCS_Quit:
		UpdateSendingQuit();
		break;
	case NetworkClientState::NCS_Platform_Update:
		UpdateSendingPlatform();
		break;
	case NetworkClientState::NCS_Player_Update:
		UpdateSendingPlayer();
		break;
	case NetworkClientState::NCS_Goal_Reached:
		UpdateSendingGoalReached();
		break;
	case NetworkClientState::NCS_Team_Change:
		UpdateSendingTeamChange();
		break;
	case NetworkClientState::NCS_Start_Network_Game:
		UpdateSendingStartGame();
		break;
	case NetworkClientState::NCS_Team_Death:
		UpdateSendingTeamDeath();
		break;
	case NetworkClientState::NCS_Checkpoint_Reached:
		UpdateSendingCheckpoint();
		break;
	case NetworkClientState::NCS_Uninitialized:
	case NetworkClientState::NCS_Size:
	case NetworkClientState::NCS_State:
	default:
		break;
	}
}

void NetworkManagerClient::UpdateSayingHello()
{
	UpdateInfoPacket(NetworkClientState::NCS_Saying_Hello, [this] { SendHelloPacket(); });
}

void NetworkManagerClient::UpdateInfoPacket(const NetworkClientState p_enum,
                                            const std::function<void()>& p_function)
{
	const float time = Timing::sInstance.GetTimef();

	if (time > mTimeOfLastPacket[p_enum] + kTimeBetweenPackets)
	{
		p_function();
		mTimeOfLastPacket[p_enum] = time;
	}
}


void NetworkManagerClient::SendHelloPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Hello);
	packet.Write(mName);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendQuitPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Quit);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPlatformPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Platform);
	packet.Write(mPlayerId);
	//packet.Write(mPlatformId);
	//packet.Write(platform);


	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPlayerPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Player);
	packet.Write(mPlayerId);
	packet.Write(mName);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendGoalPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Goal);
	packet.Write(mPlayerId);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendTeamChangePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Changed);
	packet.Write(mPlayerId);
	packet.Write(mTeamID);
	packet.Write(mColor, 1);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendStartGamePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Start_Game);
	packet.Write(mPlayerId);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendTeamDeathPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Death);
	packet.Write(mTeamID);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendCheckpointPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Checkpoint);
	packet.Write(mTeamID);
	//packet.Write(mPlatformId);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPositionPacket()
{
	// todo
}

void NetworkManagerClient::UpdateSendingQuit()
{
	UpdateInfoPacket(NetworkClientState::NCS_Quit, [this] { SendQuitPacket(); });
}


void NetworkManagerClient::UpdateSendingPlatform()
{
	UpdateInfoPacket(NetworkClientState::NCS_Platform_Update, [this] { SendPlatformPacket(); });
}

void NetworkManagerClient::UpdateSendingPlayer()
{
	UpdateInfoPacket(NetworkClientState::NCS_Player_Update, [this] { SendPlayerPacket(); });
}

void NetworkManagerClient::UpdateSendingGoalReached()
{
	UpdateInfoPacket(NetworkClientState::NCS_Goal_Reached, [this] { SendGoalPacket(); });
}

void NetworkManagerClient::UpdateSendingTeamChange()
{
	UpdateInfoPacket(NetworkClientState::NCS_Team_Change, [this] { SendTeamChangePacket(); });
}

void NetworkManagerClient::UpdateSendingStartGame()
{
	UpdateInfoPacket(NetworkClientState::NCS_Start_Network_Game, [this] { SendStartGamePacket(); });
}

void NetworkManagerClient::UpdateSendingTeamDeath()
{
	UpdateInfoPacket(NetworkClientState::NCS_Team_Death, [this] { SendTeamDeathPacket(); });
}

void NetworkManagerClient::UpdateSendingCheckpoint()
{
	UpdateInfoPacket(NetworkClientState::NCS_Checkpoint_Reached,
	                 [this] { SendCheckpointPacket(); });
}

void NetworkManagerClient::HandleWelcomePacket(InputMemoryBitStream& inInputStream)
{
	if (mState == NetworkClientState::NCS_Saying_Hello)
	{
		//if we got a player id, we've been welcomed!
		int playerId;
		inInputStream.Read(playerId);
		mPlayerId = playerId;
		mState = NetworkClientState::NCS_Welcomed;
		dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->AddPlayer(playerId, mName);
		LOG("'%s' was welcomed on client as player %d", mName.c_str(), mPlayerId)
	}
}


void NetworkManagerClient::HandleStatePacket(InputMemoryBitStream& inInputStream)
{
}

void NetworkManagerClient::ReadLastMoveProcessedOnServerTimestamp(
	InputMemoryBitStream& inInputStream)
{
	bool isTimestampDirty;
	inInputStream.Read(isTimestampDirty);
	if (isTimestampDirty)
	{
		inInputStream.Read(mLastMoveProcessedByServerTimestamp);

		const float rtt = Timing::sInstance.GetFrameStartTime() -
			mLastMoveProcessedByServerTimestamp;
		mLastRoundTripTime = rtt;
		mAvgRoundTripTime.Update(rtt);

		InputManager::sInstance->GetMoveList().RemovedProcessedMoves(
			mLastMoveProcessedByServerTimestamp);
	}
}

void NetworkManagerClient::HandleTeamChange(InputMemoryBitStream& inInputStream)
{
	int playerId;
	int teamId;
	inInputStream.Read(playerId);
	inInputStream.Read(teamId);

	if (teamId == 0)
		dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->MovePlayerBack(playerId);
	else
		dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->MovePlayer(playerId, teamId);

	mPlayerId = playerId;
	mTeamID = teamId;

	mState = NetworkClientState::NCS_State;
	LOG("'%s' changed to team %d", mName.c_str(), teamId)
}

void NetworkManagerClient::HandleGameObjectState(InputMemoryBitStream& inInputStream)
{
	//copy the mNetworkIdToGameObjectMap so that anything that doesn't get an updated can be destroyed...
	IntToGameObjectMap objectsToDestroy = m_network_id_to_game_object_map;

	int stateCount;
	inInputStream.Read(stateCount);
	if (stateCount > 0)
	{
		for (int stateIndex = 0; stateIndex < stateCount; ++stateIndex)
		{
			int networkId;
			uint32_t fourCC;

			inInputStream.Read(networkId);
			inInputStream.Read(fourCC);
			GameObjectPtr go;
			auto itGO = m_network_id_to_game_object_map.find(networkId);
			//didn't find it, better create it!
			if (itGO == m_network_id_to_game_object_map.end())
			{
				go = GameObjectRegistry::sInstance->CreateGameObject(fourCC);
				go->SetNetworkId(networkId);
				AddNetworkIdToGameObjectMap(go);
			}
			else
			{
				//found it
				go = itGO->second;
			}

			//now we can update into it
			go->Read(inInputStream);
			objectsToDestroy.erase(networkId);
		}
	}

	//anything left gets the axe
	DestroyGameObjectsInMap(objectsToDestroy);
}

void NetworkManagerClient::HandleScoreBoardState(InputMemoryBitStream& inInputStream)
{
	ScoreBoardManager::sInstance->Read(inInputStream);
}

void NetworkManagerClient::DestroyGameObjectsInMap(const IntToGameObjectMap& inObjectsToDestroy)
{
	for (auto& pair : inObjectsToDestroy)
	{
		pair.second->SetDoesWantToDie(true);
		//and remove from our map!
		m_network_id_to_game_object_map.erase(pair.first);
	}
}

void NetworkManagerClient::SetState(const NetworkClientState m_state)
{
	mState = m_state;
}


void NetworkManagerClient::UpdateSendingInputPacket()
{
	UpdateInfoUpdatePacket(NetworkClientState::NCS_Welcomed, [this] { SendInputPacket(); });
}

void NetworkManagerClient::UpdateSendingPosition()
{
	UpdateInfoUpdatePacket(NetworkClientState::NCS_Position_Update,
	                       [this] { SendPositionPacket(); });
}

void NetworkManagerClient::UpdateInfoUpdatePacket(const NetworkClientState p_enum,
                                                  const std::function<void()>& p_function)
{
	const float time = Timing::sInstance.GetTimef();

	if (time > mTimeOfLastPacket[p_enum] + kTimeBetweenUpdatePackets)
	{
		p_function();
		mTimeOfLastPacket[p_enum] = time;
	}
}

void NetworkManagerClient::SendInputPacket()
{
	OutputMemoryBitStream packet;
	packet.Write(PacketType::PT_Welcomed);
	SendPacket(packet, mServerAddress);
}

