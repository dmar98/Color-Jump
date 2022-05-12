#include "RoboCatClientPCH.hpp"

NetworkManagerClient* NetworkManagerClient::sInstance;

namespace
{
	constexpr float kTimeBetweenPackets = 1.f;
	constexpr float kTimeBetweenUpdatePackets = 0.033f;
}


NetworkManagerClient::NetworkManagerClient() :
	mDeliveryNotificationManager(true, false),
	mState(NetworkClientState::NCS_Uninitialized),
	mPlayerId(-1),
	mTeamID(-1),
	mPlatformId(-1),
	mColor(EColorType::kBlue),
	mReady(false),
	m_x(0),
	m_y(0),
	mPlatformType(EPlatformType::kNormal)
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
	int player_id;
	int team_id;
	float completion_time;
	inInputStream.Read(player_id);
	inInputStream.Read(team_id);
	inInputStream.Read(completion_time);

	if (GetMultiplayerGameState() != nullptr)
	{
		GetMultiplayerGameState()->HandleGameEnd(team_id, completion_time);
	}
	else
	{
		dynamic_cast<MultiPlayerGameOverState*>(StackManager::sInstance->GetCurrentState())->HandleGameEnd(team_id, completion_time);
	}

	if (mState == NetworkClientState::NCS_Goal_Reached && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_Game_State;
	}
}

void NetworkManagerClient::HandlePlatformPacket(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int platform_id;
	EPlatformType platform_color;

	inInputStream.Read(player_id);
	inInputStream.Read(platform_id);
	inInputStream.Read(platform_color);

	Debug("Handle Platfrom change: Platform ID:" + std::to_string(platform_id) + " Color: " + std::to_string(((int)platform_color)));

	if (GetMultiplayerGameState() != nullptr)
	{
		GetMultiplayerGameState()->HandlePlatformChange(player_id, platform_id, platform_color);
	}

	if (mState == NetworkClientState::NCS_Platform && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_Game_State;
	}
}

void NetworkManagerClient::HandleTeamDeathPacket(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int team_id;
	inInputStream.Read(player_id);
	inInputStream.Read(team_id);

	if (GetMultiplayerGameState() != nullptr)
		GetMultiplayerGameState()->HandleTeamRespawn(team_id);

	if (mState == NetworkClientState::NCS_Team_Death && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_Game_State;
	}
}

void NetworkManagerClient::HandleCheckpointPacket(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int team_id;
	int platform_id;

	inInputStream.Read(player_id);
	inInputStream.Read(team_id);
	inInputStream.Read(platform_id);
	if (GetMultiplayerGameState() != nullptr)
		GetMultiplayerGameState()->HandleTeamCheckpointSet(team_id, platform_id);

	if (mState == NetworkClientState::NCS_Checkpoint_Reached && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_Game_State;
	}
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

		if (GetMultiplayerGameState() != nullptr)
			GetMultiplayerGameState()->SpawnGhostPlayer(player_id, team_id, color, name);
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

	if (GetMultiplayerGameState() != nullptr)
		GetMultiplayerGameState()->SpawnClientPlayer(player_id, team_id, color, name);
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
	case PacketType::PT_Name:
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
		HandleGameStatePacket(inInputStream);
		break;
	case PacketType::PT_Color: 
		HandleColorChange(inInputStream);
		break;
	default:
		break;
	}
}

void NetworkManagerClient::HandleConnectionReset(const SocketAddress& inFromAddress)
{
	if (GetLobbyState())
	{
		GetLobbyState()->Quit();
	}

	if (GetMultiplayerGameState())
	{
		GetMultiplayerGameState()->Quit();
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
	case NetworkClientState::NCS_Name:
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
		UpdatePositions();
		break;
	case NetworkClientState::NCS_Team_Changed:
		UpdateTeamChanged();
		break;
	case NetworkClientState::NCS_Color:
		UpdateColor();
		break;
	case NetworkClientState::NCS_Ready:
		UpdateReady();
		break;
	case NetworkClientState::NCS_Platform:
		UpdatePlatform();
		break;
	case NetworkClientState::NCS_Uninitialized:
	case NetworkClientState::NCS_Spawn:
	case NetworkClientState::NCS_Size:
	default:
		break;
	}
}

void NetworkManagerClient::UpdatePositions()
{
	UpdateInfoUpdatePacket(NetworkClientState::NCS_Game_State, [this] { SendPositionPacket(); });
}

void NetworkManagerClient::UpdateTeamChanged()
{
	UpdateInfoPacket(NetworkClientState::NCS_Team_Changed, [this] { SendTeamPacket(); });
}

void NetworkManagerClient::UpdateColor()
{
	UpdateInfoPacket(NetworkClientState::NCS_Color, [this] { SendColorPacket(); });
}

void NetworkManagerClient::UpdateReady(const bool ready)
{
	mState = NetworkClientState::NCS_Ready;
	mReady = ready;
}

void NetworkManagerClient::UpdateSayingHello()
{
	UpdateInfoPacket(NetworkClientState::NCS_Saying_Hello, [this] { SendHelloPacket(); });
}

void NetworkManagerClient::UpdateReady()
{
	UpdateInfoPacket(NetworkClientState::NCS_Ready, [this] { SendReadyPacket(); });
}

void NetworkManagerClient::UpdatePlatform()
{
	UpdateInfoUpdatePacket(NetworkClientState::NCS_Platform, [this] { SendPlatformPacket(); });
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
	packet.Write(GetName());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendQuitPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Quit);
	packet.Write(GetPlayerId());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPlayerPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Name);
	packet.Write(GetPlayerId());
	packet.Write(GetName());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendReadyPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Ready);
	packet.Write(GetPlayerId());
	packet.Write(GetReady());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPlatformPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Platform);
	packet.Write(GetPlayerId());
	packet.Write(GetPlatformId());
	packet.Write(GetPlatformType());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendGoalPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Goal);
	packet.Write(GetPlayerId());
	packet.Write(GetTeamID());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::UpdateTeam(const int team_id, const EColorType color)
{
	mState = NetworkClientState::NCS_Team_Changed;

	mTeamID = team_id;
	mColor = color;
}

void NetworkManagerClient::UpdatePlatform(const int platform_id,
                                          const EPlatformType platform_type)
{
	if (mState != NetworkClientState::NCS_Platform)
	{
		mState = NetworkClientState::NCS_Platform;

		mPlatformId = platform_id;
		mPlatformType = platform_type;
	}
}

void NetworkManagerClient::UpdateName(const string& name)
{
	mState = NetworkClientState::NCS_Name;

	mName = name;
}

void NetworkManagerClient::UpdateColor(const EColorType color)
{
	mState = NetworkClientState::NCS_Color;

	mColor = color;
}

void NetworkManagerClient::UpdatePlayerPosition(const float x, const float y)
{
	m_x = x;
	m_y = y;
}

void NetworkManagerClient::SendStartGamePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Start_Game);
	packet.Write(GetPlayerId());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendTeamDeathPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Death);
	packet.Write(GetPlayerId());
	packet.Write(GetTeamID());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendCheckpointPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Checkpoint);
	packet.Write(GetPlayerId());
	packet.Write(GetTeamID());
	packet.Write(GetPlatformId());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendStatePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_State_Lobby);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPositionPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Game_State);
	packet.Write(GetPlayerId());
	packet.Write(m_x);
	packet.Write(m_y);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendTeamPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Changed);
	packet.Write(GetPlayerId());
	packet.Write(GetTeamID());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendColorPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Color);
	packet.Write(GetPlayerId());
	packet.Write(GetColor());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::UpdateSendingQuit()
{
	UpdateInfoPacket(NetworkClientState::NCS_Quit, [this] { SendQuitPacket(); });
}

void NetworkManagerClient::UpdateSendingPlayer()
{
	UpdateInfoPacket(NetworkClientState::NCS_Name, [this] { SendPlayerPacket(); });
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

void NetworkManagerClient::HandleWelcomePacket(InputMemoryBitStream& inInputStream)
{
	if (mState == NetworkClientState::NCS_Saying_Hello)
	{
		int playerId;
		inInputStream.Read(playerId);

		mPlayerId = playerId;
		mState = NetworkClientState::NCS_State;

		GetLobbyState()->AddPlayer(playerId, mName, false);
		GetLobbyState()->HandleConnected();

		Debug(mName + " was welcomed on client as player " + std::to_string(GetPlayerId()));
	}
}


void NetworkManagerClient::HandleStatePacket() const
{
	Debug(mName + " is waiting in the lobby.");
}

void NetworkManagerClient::HandleGameStatePacket(InputMemoryBitStream& inInputStream) const
{
	int player_count;
	int player_id;
	float x;
	float y;

	inInputStream.Read(player_count);

	for (int i = 0; i < player_count; ++i)
	{
		inInputStream.Read(player_id);
		inInputStream.Read(x);
		inInputStream.Read(y);

		if (GetMultiplayerGameState() != nullptr && player_id != GetPlayerId())
		{
			GetMultiplayerGameState()->UpdatePlayer(player_id, x, y);
		}
	}
	/*Debug(mName + " is in the game.");*/
}

void NetworkManagerClient::HandlePlayerPacket(InputMemoryBitStream& input_memory_bit_stream) const
{
	int player_id;
	string name;
	input_memory_bit_stream.Read(player_id);
	input_memory_bit_stream.Read(name);

	if (GetPlayerId() != player_id)
	{
		GetLobbyState()->AddPlayer(player_id, name, false);
		Debug(name + " was welcomed on client as player " + std::to_string(player_id));
	}
}

LobbyState* NetworkManagerClient::GetLobbyState()
{
	return dynamic_cast<LobbyState*>(StackManager::sInstance->GetCurrentState());
}

MultiplayerGameState* NetworkManagerClient::GetMultiplayerGameState()
{
	return dynamic_cast<MultiplayerGameState*>(StackManager::sInstance->GetCurrentState());
}

void NetworkManagerClient::HandleInitialStatePacket(
	InputMemoryBitStream& input_memory_bit_stream) const
{
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

		GetLobbyState()->AddPlayer(player_id, name, ready);

		if (team_id != 0)
		{
			if (player_id != GetPlayerId())
			{
				GetLobbyState()->MovePlayer(player_id, team_id);
			}
		}
	}
}

void NetworkManagerClient::HandleQuitPacket(InputMemoryBitStream& input_memory_bit_stream)
{
	int player_id;
	input_memory_bit_stream.Read(player_id);

	if (GetLobbyState())
	{
		GetLobbyState()->RemovePlayer(player_id);
	}
	else if (GetMultiplayerGameState())
	{
		GetMultiplayerGameState()->RemovePlayer(player_id);
	}
	

	if (mState == NetworkClientState::NCS_Quit && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_State;
	}
}

void NetworkManagerClient::HandlePlayerNamePacket(InputMemoryBitStream& input_memory_bit_stream)
{
	int player_id;
	std::string name;

	input_memory_bit_stream.Read(player_id);
	input_memory_bit_stream.Read(name);
	GetLobbyState()->SetName(player_id, name);

	if (mState == NetworkClientState::NCS_Name && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_State;
	}
}

void NetworkManagerClient::HandleTeamChange(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int team_id;
	inInputStream.Read(player_id);
	inInputStream.Read(team_id);

	if (team_id == 0)
		GetLobbyState()->MovePlayerBack(player_id);
	else
		GetLobbyState()->MovePlayer(player_id, team_id);

	Debug(mName + " changed to team " + std::to_string(team_id));

	if (mState == NetworkClientState::NCS_Team_Changed && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_State;
	}
}

void NetworkManagerClient::HandleColorChange(InputMemoryBitStream& inInputStream)
{
	int player_id;
	inInputStream.Read(player_id);

	if (mState == NetworkClientState::NCS_Color && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_State;
	}
}

void NetworkManagerClient::HandleStartPacket()
{
	if (mState == NetworkClientState::NCS_State)
	{
		GetLobbyState()->Start();
		mState = NetworkClientState::NCS_Spawn;
	}
}

void NetworkManagerClient::HandleStartCountdownPacket()
{
	GetLobbyState()->StartCountDown();
}

void NetworkManagerClient::HandleReadyChange(InputMemoryBitStream& input_memory_bit_stream)
{
	int player_id;
	bool ready;

	input_memory_bit_stream.Read(player_id);
	input_memory_bit_stream.Read(ready);

	GetLobbyState()->SetReady(player_id, ready);

	if (mState == NetworkClientState::NCS_Ready && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_State;
	}
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
