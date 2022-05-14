#include "ColorJumpClientPCH.hpp"

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
	mTeamID(0),
	mPlatformId(-1),
	mColor(kBlue),
	mReady(false),
	m_x(0),
	m_y(0),
	mPlatformType(kNormal)
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
	int player_id = 0;
	int team_id = 0;
	float completion_time = 0;
	inInputStream.Read(player_id, 5);
	inInputStream.Read(team_id, 4);
	inInputStream.Read(completion_time);

	if (GetMultiplayerGameState() != nullptr)
	{
		GetMultiplayerGameState()->HandleGameEnd(team_id, completion_time);
	}
	else
	{
		dynamic_cast<MultiPlayerGameOverState*>(StackManager::sInstance->GetCurrentState())->
			HandleGameEnd(team_id, completion_time);
	}

	if (mState == NetworkClientState::NCS_Goal_Reached && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_Game_State;
	}
}

void NetworkManagerClient::HandlePlatformPacket(InputMemoryBitStream& inInputStream)
{
	int player_id = 0;
	int platform_id = 0;
	auto platform_color = static_cast<EPlatformType>(0);

	inInputStream.Read(player_id, 5);
	inInputStream.Read(platform_id);
	inInputStream.Read(platform_color);

	Debug("Handle Platform change: Platform ID:" + std::to_string(platform_id) + " Color: " +
		std::to_string(static_cast<int>(platform_color)));

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
	int player_id = 0;
	int team_id = 0;
	inInputStream.Read(player_id, 5);
	inInputStream.Read(team_id, 4);

	if (GetMultiplayerGameState() != nullptr)
		GetMultiplayerGameState()->HandleTeamRespawn(team_id);

	if (mState == NetworkClientState::NCS_Team_Death && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_Game_State;
	}
}

void NetworkManagerClient::HandleCheckpointPacket(InputMemoryBitStream& inInputStream)
{
	int player_id = 0;
	int team_id = 0;
	int platform_id = 0;

	inInputStream.Read(player_id, 5);
	inInputStream.Read(team_id, 4);
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
	int player_id = 0;
	int team_id = 0;
	auto color = static_cast<EColorType>(0);
	std::string name;
	int player_count = 0;

	inInputStream.Read(player_count, 5);

	for (int i = 0; i < player_count; ++i)
	{
		inInputStream.Read(player_id, 5);
		inInputStream.Read(team_id, 4);
		inInputStream.Read(color, 1);
		inInputStream.Read(name);

		if (GetMultiplayerGameState() != nullptr)
			GetMultiplayerGameState()->SpawnGhostPlayer(player_id, team_id, color, name);
	}
}

void NetworkManagerClient::ReadPlayerData(InputMemoryBitStream& inInputStream)
{
	int player_id = 0;
	int team_id = 0;
	auto color = static_cast<EColorType>(0);
	std::string name;
	inInputStream.Read(player_id, 5);
	inInputStream.Read(team_id, 4);
	inInputStream.Read(color, 1);
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
	auto packetType = static_cast<PacketType>(0);
	inInputStream.Read(packetType, 5);
	switch (packetType)
	{
	case PT_Hello:
		HandleWelcomePacket(inInputStream);
		break;
	case PT_State_Lobby:
		HandleStatePacket();
		break;
	case PT_Initial_State:
		HandleInitialStatePacket(inInputStream);
		break;
	case PT_Quit:
		HandleQuitPacket(inInputStream);
		break;
	case PT_Player_Connect:
		HandlePlayerPacket(inInputStream);
		break;
	case PT_Name:
		HandlePlayerNamePacket(inInputStream);
		break;
	case PT_Team_Changed:
		HandleTeamChange(inInputStream);
		break;
	case PT_Start_Game:
		HandleStartPacket();
		break;
	case PT_Start_Game_Countdown:
		HandleStartCountdownPacket();
		break;
	case PT_Goal:
		HandleGameWon(inInputStream);
		break;
	case PT_Platform:
		HandlePlatformPacket(inInputStream);
		break;
	case PT_Team_Death:
		HandleTeamDeathPacket(inInputStream);
		break;
	case PT_Checkpoint:
		HandleCheckpointPacket(inInputStream);
		break;
	case PT_Spawn:
		HandleSpawnPacket(inInputStream);
		break;
	case PT_Ready:
		HandleReadyChange(inInputStream);
		break;
	case PT_Game_State:
		HandleGameStatePacket(inInputStream);
		break;
	case PT_Color:
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
	else
	{
		dynamic_cast<MultiPlayerGameOverState*>(StackManager::sInstance->GetCurrentState())->Quit();
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

	packet.Write(PT_Hello, 5);
	packet.Write(GetName());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendQuitPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Quit, 5);
	packet.Write(GetPlayerId(), 5);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendNamePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Name, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetName());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendReadyPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Ready, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetReady());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPlatformPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Platform, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetPlatformId());
	packet.Write(GetPlatformType());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendGoalPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Goal, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetTeamID(), 4);

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

	packet.Write(PT_Start_Game, 5);
	packet.Write(GetPlayerId(), 5);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendTeamDeathPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Team_Death, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetTeamID(), 4);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendCheckpointPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Checkpoint, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetTeamID(), 4);
	packet.Write(GetPlatformId());

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendStatePacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_State_Lobby, 5);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendPositionPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Game_State, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(m_x);
	packet.Write(m_y);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendTeamPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Team_Changed, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetTeamID(), 4);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::SendColorPacket()
{
	OutputMemoryBitStream packet;

	packet.Write(PT_Color, 5);
	packet.Write(GetPlayerId(), 5);
	packet.Write(GetColor(), 1);

	SendPacket(packet, mServerAddress);
}

void NetworkManagerClient::UpdateSendingQuit()
{
	UpdateInfoPacket(NetworkClientState::NCS_Quit, [this] { SendQuitPacket(); });
}

void NetworkManagerClient::UpdateSendingPlayer()
{
	UpdateInfoPacket(NetworkClientState::NCS_Name, [this] { SendNamePacket(); });
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
		int player_id = 0;
		inInputStream.Read(player_id, 5);

		mPlayerId = player_id;
		mState = NetworkClientState::NCS_State;

		GetLobbyState()->AddPlayer(player_id, mName, false);
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
	int player_count = 0;
	int player_id = 0;
	float x = 0;
	float y = 0;

	inInputStream.Read(player_count, 5);

	for (int i = 0; i < player_count; ++i)
	{
		inInputStream.Read(player_id, 5);
		inInputStream.Read(x);
		inInputStream.Read(y);

		if (GetMultiplayerGameState() != nullptr && player_id != GetPlayerId())
		{
			GetMultiplayerGameState()->UpdatePlayer(player_id, x, y);
		}
	}
	/*Debug(mName + " is in the game.");*/
}

void NetworkManagerClient::HandlePlayerPacket(InputMemoryBitStream& inInputStream) const
{
	int player_id = 0;
	string name;

	inInputStream.Read(player_id, 5);
	inInputStream.Read(name);

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
	InputMemoryBitStream& inInputStream) const
{
	int player_count = 0;
	inInputStream.Read(player_count, 5);
	for (int i = 0; i < player_count; ++i)
	{
		int player_id = 0;
		int team_id = 0;
		std::string name;
		bool ready = false;
		inInputStream.Read(player_id, 5);
		inInputStream.Read(team_id, 4);
		inInputStream.Read(name);
		inInputStream.Read(ready);

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

void NetworkManagerClient::HandleQuitPacket(InputMemoryBitStream& inInputStream)
{
	int player_id = 0;
	inInputStream.Read(player_id, 5);

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

void NetworkManagerClient::HandlePlayerNamePacket(InputMemoryBitStream& inInputStream)
{
	int player_id = 0;
	std::string name;

	inInputStream.Read(player_id, 5);
	inInputStream.Read(name);
	GetLobbyState()->SetName(player_id, name);

	if (mState == NetworkClientState::NCS_Name && player_id == GetPlayerId())
	{
		mState = NetworkClientState::NCS_State;
	}
}

void NetworkManagerClient::HandleTeamChange(InputMemoryBitStream& inInputStream)
{
	int player_id = 0;
	int team_id = 0;
	inInputStream.Read(player_id, 5);
	inInputStream.Read(team_id, 4);

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
	int player_id = 0;
	inInputStream.Read(player_id, 5);

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

void NetworkManagerClient::HandleReadyChange(InputMemoryBitStream& inInputStream)
{
	uint8_t player_id = 0;
	bool ready = false;

	inInputStream.Read(player_id, 5);
	inInputStream.Read(ready);

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
	packet.Write(PT_State_Lobby, 5);
	SendPacket(packet, mServerAddress);
}
