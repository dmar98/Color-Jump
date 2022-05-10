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

void NetworkManagerClient::SetPlayerColor(const EColorType color)
{
	mColor = color;
}

void NetworkManagerClient::SendReadyPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Ready);
	packet.Write(GetPlayerId());
	packet.Write(mReady);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendReady(const bool ready)
{
	mReady = ready;
	SendReadyPacket();
}

NetworkManagerClient::NetworkManagerClient() :
	mDeliveryNotificationManager(true, false),
	mColor(EColorType::kBlue)
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

void NetworkManagerClient::HandleGameWon(InputMemoryBitStream& inInputStream)
{
	int team_id;
	inInputStream.Read(team_id);

	dynamic_cast<MultiplayerGameState*>(StackManager::sInstance->GetCurrentState())->
		HandleGameEnd(team_id);
}

void NetworkManagerClient::HandlePlatformPacket(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int platform_id;
	EPlatformType platform_color;

	inInputStream.Read(player_id);
	inInputStream.Read(platform_id);
	inInputStream.Read(platform_color);

	dynamic_cast<MultiplayerGameState*>(StackManager::sInstance->GetCurrentState())->
		HandlePlatformChange(player_id, platform_id, platform_color);
}

void NetworkManagerClient::HandleTeamDeathPacket(InputMemoryBitStream& inInputStream)
{
	int team_id;
	inInputStream.Read(team_id);

	dynamic_cast<MultiplayerGameState*>(StackManager::sInstance->GetCurrentState())->
		HandleTeamRespawn(team_id);
}

void NetworkManagerClient::HandleCheckpointPacket(InputMemoryBitStream& inInputStream)
{
	int team_id;
	int platform_id;

	inInputStream.Read(team_id);
	inInputStream.Read(platform_id);

	dynamic_cast<MultiplayerGameState*>(StackManager::sInstance->GetCurrentState())->
		HandleTeamCheckpointSet(team_id, platform_id);
}

void NetworkManagerClient::ReadGhostData(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int team_id;
	EColorType color;
	std::string name;
	int player_count;

	inInputStream.Read(player_count);

	for (int i = 0; i < player_count; ++i)
	{
		inInputStream.Read(player_id);
		inInputStream.Read(team_id);
		inInputStream.Read(color);
		inInputStream.Read(name);

		dynamic_cast<MultiplayerGameState*>(StackManager::sInstance->GetCurrentState())->
			SpawnGhostPlayer(player_id, team_id, color, name);
	}
}

void NetworkManagerClient::ReadPlayerData(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int team_id;
	EColorType color;
	std::string name;
	inInputStream.Read(player_id);
	inInputStream.Read(team_id);
	inInputStream.Read(color);
	inInputStream.Read(name);

	dynamic_cast<MultiplayerGameState*>(StackManager::sInstance->GetCurrentState())->
		SpawnClientPlayer(player_id, team_id, color, name);
}

void NetworkManagerClient::HandleSpawnPacket(InputMemoryBitStream& inInputStream)
{
	if (mState == NetworkClientState::NCS_Spawn)
	{
		mState = NetworkClientState::NCS_Game_State;
		ReadGhostData(inInputStream);
		ReadPlayerData(inInputStream);
	}
}

void NetworkManagerClient::ProcessPacket(InputMemoryBitStream& inInputStream,
                                         const SocketAddress& inFromAddress)
{
	PacketType packetType;
	inInputStream.Read(packetType);
	switch (packetType)
	{
	case PacketType::PT_Hello:
		HandleWelcomePacket(inInputStream);
		break;
	case PacketType::PT_State_Lobby:
		HandleStatePacket();
		break;
	case PacketType::PT_Initial_State:
		HandleInitialStatePacket(inInputStream);
		break;
	case PacketType::PT_Quit:
		HandleQuitPacket(inInputStream);
		break;
	case PacketType::PT_Player_Connect:
		HandlePlayerPacket(inInputStream);
		break;
	case PacketType::PT_Player:
		HandlePlayerNamePacket(inInputStream);
		break;
	case PacketType::PT_Team_Changed:
		HandleTeamChange(inInputStream);
		break;
	case PacketType::PT_Start_Game:
		HandleStartPacket();
		break;
	case PacketType::PT_Start_Game_Countdown:
		HandleStartCountdownPacket();
		break;
	case PacketType::PT_Goal:
		HandleGameWon(inInputStream);
		break;
	case PacketType::PT_Platform:
		HandlePlatformPacket(inInputStream);
		break;
	case PacketType::PT_Team_Death:
		HandleTeamDeathPacket(inInputStream);
		break;
	case PacketType::PT_Checkpoint:
		HandleCheckpointPacket(inInputStream);
		break;
	case PacketType::PT_Spawn:
		HandleSpawnPacket(inInputStream);
		break;
	case PacketType::PT_Ready:
		HandleReadyChange(inInputStream);
		break;
	case PacketType::PT_Game_State: 
		HandleGameStatePacket();
		break;
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
	case NetworkClientState::NCS_Quit:
		UpdateSendingQuit();
		break;
	case NetworkClientState::NCS_Player_Update:
		UpdateSendingPlayer();
		break;
	case NetworkClientState::NCS_Goal_Reached:
		UpdateSendingGoalReached();
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
	case NetworkClientState::NCS_State:
		UpdateSendingWaitState();
		break;
	case NetworkClientState::NCS_Game_State:
		UpdateSendingGameState();
		break;
	case NetworkClientState::NCS_Uninitialized:
	case NetworkClientState::NCS_Spawn:
	case NetworkClientState::NCS_Size:
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
	packet.Write(mColor);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPlatformInfo(const int player_id, const int platform_id,
                                            const EPlatformType platform_type)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Platform);
	packet.Write(player_id);
	packet.Write(platform_id);
	packet.Write(platform_type);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendCheckpointReached(const int team_id, int platform_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Checkpoint);
	packet.Write(team_id);
	packet.Write(platform_id);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendGoalReached(int team_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Goal);
	packet.Write(team_id);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendTeamDeath(const int team_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Death);
	packet.Write(team_id);

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

void NetworkManagerClient::SendStatePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_State_Lobby);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendGameStatePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Game_State);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::UpdateSendingQuit()
{
	UpdateInfoPacket(NetworkClientState::NCS_Quit, [this] { SendQuitPacket(); });
}

void NetworkManagerClient::UpdateSendingPlayer()
{
	UpdateInfoPacket(NetworkClientState::NCS_Player_Update, [this] { SendPlayerPacket(); });
}

void NetworkManagerClient::UpdateSendingGoalReached()
{
	UpdateInfoPacket(NetworkClientState::NCS_Goal_Reached, [this] { SendGoalPacket(); });
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

void NetworkManagerClient::UpdateSendingWaitState()
{
	UpdateInfoPacket(NetworkClientState::NCS_State,
	                 [this] { SendStatePacket(); });
}

void NetworkManagerClient::UpdateSendingGameState()
{
	UpdateInfoUpdatePacket(NetworkClientState::NCS_Game_State,
		[this] { SendGameStatePacket(); });
}

void NetworkManagerClient::HandleWelcomePacket(InputMemoryBitStream& inInputStream)
{
	if (mState == NetworkClientState::NCS_Saying_Hello)
	{
		//if we got a player id, we've been welcomed!
		int playerId;
		inInputStream.Read(playerId);
		mPlayerId = playerId;
		mState = NetworkClientState::NCS_State;
		AddPlayer(playerId, mName);
		LOG("'%s' was welcomed on client as player %d", mName.c_str(), mPlayerId)
	}
}


void NetworkManagerClient::HandleStatePacket() const
{
	LOG("'%s' is waiting in the lobby", mName.c_str())
}

void NetworkManagerClient::HandleGameStatePacket() const
{
	LOG("'%s' is in the game", mName.c_str())
}

void NetworkManagerClient::HandlePlayerPacket(InputMemoryBitStream& input_memory_bit_stream) const
{
	int player_id;
	string name;
	input_memory_bit_stream.Read(player_id);
	input_memory_bit_stream.Read(name);
	AddPlayer(player_id, name);
	LOG("'%s' was welcomed on client as player %d", name.c_str(), player_id)
}

void NetworkManagerClient::AddPlayer(const int player_id, const std::string& name, const bool ready)
{
	dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->AddPlayer(
		player_id, name, ready);
}

void NetworkManagerClient::MovePlayer(const int player_id, const int team_id)
{
	dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->MovePlayer(
		player_id, team_id);
}

void NetworkManagerClient::HandleInitialStatePacket(
	InputMemoryBitStream& input_memory_bit_stream)
{
	mState = NetworkClientState::NCS_State;

	int player_count;
	input_memory_bit_stream.Read(player_count);
	for (int i = 0; i < player_count; ++i)
	{
		int player_id;
		int team_id;
		std::string name;
		bool ready;
		input_memory_bit_stream.Read(player_id);
		input_memory_bit_stream.Read(team_id);
		input_memory_bit_stream.Read(name);
		input_memory_bit_stream.Read(ready);

		AddPlayer(player_id, name, ready);

		if (team_id != 0)
		{
			if (player_id != GetPlayerId())
			{
				MovePlayer(player_id, team_id);
			}
		}
	}
}

void NetworkManagerClient::RemovePlayer(const int player_id)
{
	dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->RemovePlayer(player_id);
}

void NetworkManagerClient::HandleQuitPacket(InputMemoryBitStream& input_memory_bit_stream)
{
	int player_id;
	input_memory_bit_stream.Read(player_id);

	RemovePlayer(player_id);
}

void NetworkManagerClient::SetName(const int player_id, const std::string& name)
{
	dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->SetName(player_id, name);
}

void NetworkManagerClient::HandlePlayerNamePacket(
	InputMemoryBitStream& input_memory_bit_stream) const
{
	int player_id;
	std::string name;

	input_memory_bit_stream.Read(player_id);
	input_memory_bit_stream.Read(name);

	SetName(player_id, name);
}


void NetworkManagerClient::MovePlayerBack(const int playerId)
{
	dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->MovePlayerBack(
		playerId);
}

void NetworkManagerClient::HandleTeamChange(InputMemoryBitStream& inInputStream)
{
	mState = NetworkClientState::NCS_State;

	int playerId;
	int teamId;
	inInputStream.Read(playerId);
	inInputStream.Read(teamId);

	if (teamId == 0)
		MovePlayerBack(playerId);
	else
		MovePlayer(playerId, teamId);

	if (playerId == GetPlayerId())
	{
		mTeamID = teamId;
	}

	LOG("'%s' changed to team %d", mName.c_str(), teamId)
}

void NetworkManagerClient::HandleStartPacket()
{
	if (mState == NetworkClientState::NCS_State)
	{
		dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->Start();
		mState = NetworkClientState::NCS_Spawn;
	}
}

void NetworkManagerClient::HandleStartCountdownPacket()
{
	dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->StartCountDown();
}

void NetworkManagerClient::HandleReadyChange(InputMemoryBitStream& input_memory_bit_stream)
{
	mState = NetworkClientState::NCS_State;

	int player_id;
	bool ready;

	input_memory_bit_stream.Read(player_id);
	input_memory_bit_stream.Read(ready);

	dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState())->SetReady(
		player_id, ready);
}


void NetworkManagerClient::SetState(const NetworkClientState m_state)
{
	mState = m_state;
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
	packet.Write(PacketType::PT_State_Lobby);
	SendPacket(packet, mServerAddress);
}
