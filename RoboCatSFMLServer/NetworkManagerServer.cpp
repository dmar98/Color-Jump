#include "RoboCatServerPCH.hpp"

NetworkManagerServer* NetworkManagerServer::sInstance;


NetworkManagerServer::NetworkManagerServer() :
	mNewPlayerId(1),
	mNewNetworkId(1),
	mClientDisconnectTimeout(3.f),
	m_start_countdown(false),
	m_start_countdown_timer(5),
	m_game_started(false)
{
}

bool NetworkManagerServer::StaticInit(const uint16_t inPort)
{
	sInstance = new NetworkManagerServer();
	return sInstance->Init(inPort);
}

void NetworkManagerServer::HandleConnectionReset(const SocketAddress& inFromAddress)
{
	//just dc the client right away...
	const auto it = mAddressToClientMap.find(inFromAddress);
	if (it != mAddressToClientMap.end())
	{
		HandleClientDisconnected(it->second);
	}
}

void NetworkManagerServer::ProcessPacket(InputMemoryBitStream& inInputStream,
                                         const SocketAddress& inFromAddress)
{
	//try to get the client proxy for this address
	//pass this to the client proxy to process
	const auto it = mAddressToClientMap.find(inFromAddress);
	if (it == mAddressToClientMap.end())
	{
		//didn't find one? it's a new client..is the a HELO? if so, create a client proxy...
		HandlePacketFromNewClient(inInputStream, inFromAddress);
	}
	else
	{
		ProcessPacket((*it).second, inInputStream);
	}
}


void NetworkManagerServer::HandlePlayerNameChange(const ClientProxyPtr& client_proxy,
                                                  InputMemoryBitStream& input_memory_bit_stream)
{
	int player_id;
	std::string name;

	input_memory_bit_stream.Read(player_id);
	input_memory_bit_stream.Read(name);
	name = name.substr(0, 20);

	client_proxy->SetName(name);

	auto p_function = [this, player_id, name](const ClientProxyPtr& client)
	{
		NotifyPlayerNameChange(client, player_id, name);
	};

	SendPacketToAll(p_function);
}

void NetworkManagerServer::HandleGoalReached(InputMemoryBitStream& inInputStream)
{
	int team_id;
	inInputStream.Read(team_id);

	auto p_function = [this, team_id](const ClientProxyPtr& client)
	{
		NotifyGoalReached(client, team_id);
	};

	SendPacketToAll(p_function);
}

void NetworkManagerServer::HandleTeamDeath(InputMemoryBitStream& inInputStream)
{
	int team_id;
	inInputStream.Read(team_id);

	auto p_function = [this, team_id](const ClientProxyPtr& client)
	{
		NotifyTeamRespawn(client, team_id);
	};

	SendPacketToAll(p_function);
}

void NetworkManagerServer::HandleCheckpoint(InputMemoryBitStream& inInputStream)
{
	int team_id;
	int platform_id;
	inInputStream.Read(team_id);
	inInputStream.Read(platform_id);

	auto p_function = [this, team_id, platform_id](const ClientProxyPtr& client)
	{
		NotifyCheckpointReached(client, team_id, platform_id);
	};

	SendPacketToAll(p_function);
}

void NetworkManagerServer::HandlePlatformChange(InputMemoryBitStream& inInputStream)
{
	int player_id;
	int platform_id;
	EPlatformType platform_color;

	inInputStream.Read(player_id);
	inInputStream.Read(platform_id);
	inInputStream.Read(platform_color);

	auto p_function = [this, player_id, platform_id, platform_color](const ClientProxyPtr& client)
	{
		NotifyPlatformUpdate(client, player_id, platform_id, platform_color);
	};

	SendPacketToAll(p_function);
}

void NetworkManagerServer::NotifyCountDown(const ClientProxyPtr& client)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Start_Game_Countdown);

	SendPacket(packet, client->GetSocketAddress());
}

void NetworkManagerServer::CheckIfAllReady()
{
	if (mAddressToClientMap.size() < 2)
	{
		return;
	}

	bool isReady = true;
	for (const auto& it : mAddressToClientMap)
	{
		if (!it.second->GetReady())
		{
			isReady = false;
			break;
		}
	}

	if (isReady)
	{
		auto p_function = [this](const ClientProxyPtr& client)
		{
			NotifyCountDown(client);
		};

		SendPacketToAll(p_function);

		m_start_countdown = true;
	}
}

void NetworkManagerServer::HandleReadyPacket(const ClientProxyPtr& client_proxy,
                                             InputMemoryBitStream& inInputStream)
{
	int player_id;
	bool ready;

	inInputStream.Read(player_id);
	inInputStream.Read(ready);

	client_proxy->SetIsReady(ready);

	auto p_function = [this, player_id, ready](const ClientProxyPtr& client)
	{
		NotifyReadyChange(client, player_id, ready);
	};

	SendPacketToAll(p_function);

	CheckIfAllReady();
}

void NetworkManagerServer::HandleGameStatePacket(const ClientProxyPtr& client_proxy,
                                                 InputMemoryBitStream& inInputStream)
{
	int player_id;
	float x;
	float y;

	inInputStream.Read(player_id);
	inInputStream.Read(x);
	inInputStream.Read(y);

	client_proxy->SetPosition(x, y);

	SendGameStatePacket(client_proxy, inInputStream);
}

void NetworkManagerServer::ProcessPacket(const ClientProxyPtr& inClientProxy,
                                         InputMemoryBitStream& inInputStream)
{
	//remember we got a packet so we know not to disconnect for a bit
	inClientProxy->UpdateLastPacketTime();

	PacketType packetType;
	inInputStream.Read(packetType);
	switch (packetType)
	{
	case PacketType::PT_Hello:
		SendWelcomePacket(inClientProxy);
		break;
	case PacketType::PT_State_Lobby:
		SendStatePacketToClient(inClientProxy);
		break;
	case PacketType::PT_Quit:
		HandleClientDisconnected(inClientProxy);
		break;
	case PacketType::PT_Team_Changed:
		HandleTeamChange(inClientProxy, inInputStream);
		break;
	case PacketType::PT_Platform:
		HandlePlatformChange(inInputStream);
		break;
	case PacketType::PT_Player:
		HandlePlayerNameChange(inClientProxy, inInputStream);
		break;
	case PacketType::PT_Goal:
		HandleGoalReached(inInputStream);
		break;
	case PacketType::PT_Team_Death:
		HandleTeamDeath(inInputStream);
		break;
	case PacketType::PT_Checkpoint:
		HandleCheckpoint(inInputStream);
		break;
	case PacketType::PT_Ready:
		HandleReadyPacket(inClientProxy, inInputStream);
		break;
	case PacketType::PT_Game_State:
		if (inClientProxy->GetDeliveryNotificationManager().ReadAndProcessState(inInputStream))
		{
			HandleGameStatePacket(inClientProxy, inInputStream);
		}
		break;
	case PacketType::PT_Player_Connect:
	case PacketType::PT_Initial_State:
	case PacketType::PT_Start_Game:
	case PacketType::PT_Start_Game_Countdown:
	case PacketType::PT_Spawn:
		break;
	default:
		LOG("Unknown packet type received from %s",
		    inClientProxy->GetSocketAddress().ToString().c_str())
		break;
	}
}

void NetworkManagerServer::SendInitialState(const ClientProxyPtr& client_proxy)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Initial_State);
	const int size = mPlayerIdToClientMap.size();
	packet.Write(size);

	for (const auto& player : mPlayerIdToClientMap)
	{
		packet.Write(player.second->GetPlayerId());
		packet.Write(player.second->GetTeamID());
		packet.Write(player.second->GetName());
		packet.Write(player.second->GetReady());
	}

	LOG("Server Initial State", 0)

	SendPacket(packet, client_proxy->GetSocketAddress());
}

void NetworkManagerServer::NotifyOfTeamChange(const ClientProxyPtr& inClientProxy,
                                              const int player_id, const int team_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Changed);
	packet.Write(player_id);
	packet.Write(team_id);

	LOG("Server Team Change, client '%s' as player %d changed to team %d",
	    inClientProxy->GetName().c_str(),
	    inClientProxy->GetPlayerId(), inClientProxy->GetTeamID())

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::NotifyPlayerNameChange(const ClientProxyPtr& inClientProxy,
                                                  const int player_id, const string& name)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Player);
	packet.Write(player_id);
	packet.Write(name);

	LOG("Server Name Change, %d is now called '%s'", player_id, name.c_str())

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::NotifyGoalReached(const ClientProxyPtr& inClientProxy,
                                             const int team_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Goal);
	packet.Write(team_id);

	LOG("Server Goal Reached, Team %d has reached the end", team_id)

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::NotifyTeamRespawn(const ClientProxyPtr& inClientProxy,
                                             const int team_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Death);
	packet.Write(team_id);

	LOG("Server Team Member Death, Member from team %d has died", team_id)

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::NotifyCheckpointReached(const ClientProxyPtr& inClientProxy,
                                                   const int team_id, const int platform_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Checkpoint);
	packet.Write(team_id);
	packet.Write(platform_id);

	LOG("Server Checpoint: Team %d has reached checkpoint with id %d", team_id, platform_id)

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::NotifyPlatformUpdate(const ClientProxyPtr& inClientProxy,
                                                const int player_id, const int platform_id,
                                                const EPlatformType platform_color)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Platform);
	packet.Write(player_id);
	packet.Write(platform_id);
	packet.Write(platform_color);

	LOG("Server Platform: Player %d has stepped on platform %d", player_id,
	    platform_id)

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::NotifyReadyChange(const ClientProxyPtr& inClientProxy,
                                             const int player_id, const bool ready)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Ready);
	packet.Write(player_id);
	packet.Write(ready);

	LOG("Server Player Ready, Player %d has changed his readiness to %d", player_id,
	    ready)

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::HandleTeamChange(const ClientProxyPtr& inClientProxy,
                                            InputMemoryBitStream& in_input_stream)
{
	int player_id;
	int team_id;
	EColorType color;

	in_input_stream.Read(player_id);
	in_input_stream.Read(team_id);
	in_input_stream.Read(color);

	inClientProxy->SetTeamID(team_id);
	inClientProxy->SetColor(color);

	LOG("Server Team Change: Player %d has changed his team to %d. He has the color %d", player_id,
	    team_id, color)

	auto p_function = [this, player_id, team_id](const ClientProxyPtr& client_proxy)
	{
		NotifyOfTeamChange(client_proxy, player_id, team_id);
	};

	SendPacketToAll(p_function);
}


void NetworkManagerServer::HandlePacketFromNewClient(InputMemoryBitStream& inInputStream,
                                                     const SocketAddress& inFromAddress)
{
	//read the beginning- is it a hello?
	PacketType packetType;
	inInputStream.Read(packetType);
	if (packetType == PacketType::PT_Hello)
	{
		//read the name
		string name;
		inInputStream.Read(name);
		const auto newClientProxy = std::make_shared<ClientProxy>(
			inFromAddress, name, mNewPlayerId++);
		mAddressToClientMap[inFromAddress] = newClientProxy;
		mPlayerIdToClientMap[newClientProxy->GetPlayerId()] = newClientProxy;

		//tell the server about this client, spawn a cat, etc...
		//if we had a generic message system, this would be a good use for it...
		//instead we'll just tell the server directly
		dynamic_cast<Server*>(Engine::s_instance.get())->HandleNewClient(newClientProxy);

		//and welcome the client...
		SendWelcomePacket(newClientProxy);

		//and now init the replication manager with everything we know about!
		for (const auto& pair : m_network_id_to_game_object_map)
		{
			newClientProxy->GetReplicationManagerServer().ReplicateCreate(
				pair.first, pair.second->GetAllStateMask());
		}
	}
	else
	{
		//bad incoming packet from unknown client- we're under attack!!
		LOG("Bad incoming packet from unknown client at socket %s: %s",
		    inFromAddress.ToString().c_str(), packetType)
	}
}

void NetworkManagerServer::SendWelcomePacket(const ClientProxyPtr& inClientProxy)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Hello);
	int player_id = inClientProxy->GetPlayerId();
	packet.Write(player_id);

	const string name = inClientProxy->GetName();
	LOG("Server Welcoming, new client '%s' as player %d", name.c_str(),
	    player_id)

	SendPacket(packet, inClientProxy->GetSocketAddress());


	SendInitialState(inClientProxy);

	auto p_function = [this, player_id, name](const ClientProxyPtr& client)
	{
		NotifyPlayerJoin(client, player_id, name);
	};

	SendPacketToAll(p_function);
}

void NetworkManagerServer::NotifyPlayerJoin(const ClientProxyPtr& inClientProxy,
                                            const int player_id, const string& name)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Player_Connect);
	packet.Write(player_id);
	packet.Write(name);

	LOG("Server Player Join, Player %d joined the server", player_id)

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::SendPacketToAll(
	const std::function<void(const ClientProxyPtr& inClientProxy)>& p_function) const
{
	for (const auto& it : mAddressToClientMap)
	{
		//process any timed out packets while we're going through the list
		it.second->GetDeliveryNotificationManager().ProcessTimedOutPackets();

		p_function(it.second);
	}
}


void NetworkManagerServer::SendStatePacketToClient(const ClientProxyPtr& inClientProxy)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_State_Lobby);

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::SendGameStatePacket(const ClientProxyPtr& inClientProxy,
                                               InputMemoryBitStream& inInputStream)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Game_State);

	const int in_data = mPlayerIdToClientMap.size();
	packet.Write(in_data);

	for (const auto& client : mPlayerIdToClientMap)
	{
		packet.Write(client.second->GetPlayerId());
		packet.Write(client.second->GetPosition().x);
		packet.Write(client.second->GetPosition().y);
	}

	SendPacket(packet, inClientProxy->GetSocketAddress());
}

//should we ask the server for this? or run through the world ourselves?
void NetworkManagerServer::AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream)
{
	const auto& gameObjects = World::sInstance->GetGameObjects();

	//now start writing objects- do we need to remember how many there are? we can check first...
	inOutputStream.Write(gameObjects.size());

	for (const GameObjectPtr& gameObject : gameObjects)
	{
		inOutputStream.Write(gameObject->GetNetworkId());
		inOutputStream.Write(gameObject->GetClassId());
		gameObject->Write(inOutputStream, 0xffffffff);
	}
}


int NetworkManagerServer::GetNewNetworkId()
{
	const int toRet = mNewNetworkId++;
	if (mNewNetworkId < toRet)
	{
		LOG("Network ID Wrap Around!!! You've been playing way too long...", 0)
	}

	return toRet;
}

ClientProxyPtr NetworkManagerServer::GetClientProxy(const int inPlayerId) const
{
	const auto it = mPlayerIdToClientMap.find(inPlayerId);
	if (it != mPlayerIdToClientMap.end())
	{
		return it->second;
	}

	return nullptr;
}

void NetworkManagerServer::Countdown(const float dt)
{
	if (m_start_countdown && !m_game_started)
	{
		if (m_start_countdown_timer > 0)
		{
			m_start_countdown_timer -= dt;
		}
		else
		{
			m_game_started = true;

			auto p_function = [this](const ClientProxyPtr& client)
			{
				NotifyGameStart(client);
			};

			SendPacketToAll(p_function);

			Sleep(500);
			NotifySpawnCharacters();
		}
	}
}

void NetworkManagerServer::NotifyGameStart(const ClientProxyPtr& client)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Start_Game);

	SendPacket(packet, client->GetSocketAddress());
}

void NetworkManagerServer::NotifySpawnCharacters()
{
	for (const auto& it : mAddressToClientMap)
	{
		//process any timed out packets while we're going through the list
		it.second->GetDeliveryNotificationManager().ProcessTimedOutPackets();

		OutputMemoryBitStream packet;

		packet.Write(PacketType::PT_Spawn);
		const int size = mPlayerIdToClientMap.size();
		packet.Write(size - 1);

		for (int i = 0; i < size; ++i)
		{
			int id = it.second->GetPlayerId();

			for (const auto& player_info : mPlayerIdToClientMap)
			{
				if (player_info.first == id)
				{
					continue;
				}

				const shared_ptr<ClientProxy> client_proxy = player_info.second;

				packet.Write(player_info.first);
				packet.Write(client_proxy->GetTeamID());
				LOG("Server Color Sent Ghost: %d", client_proxy->GetColor())
				packet.Write(client_proxy->GetColor());
				packet.Write(client_proxy->GetName());
			}

			const shared_ptr<ClientProxy> proxy = mPlayerIdToClientMap[id];

			packet.Write(id);
			packet.Write(proxy->GetTeamID());
			LOG("Server Color Sent Client: %d", proxy->GetColor())
			packet.Write(proxy->GetColor());
			packet.Write(proxy->GetName());

			SendPacket(packet, it.second->GetSocketAddress());
		}
	}
}

void NetworkManagerServer::CheckForDisconnects()
{
	vector<ClientProxyPtr> clientsToDC;

	const float minAllowedLastPacketFromClientTime = Timing::sInstance.GetTimef() -
		mClientDisconnectTimeout;
	for (const auto& pair : mAddressToClientMap)
	{
		if (pair.second->GetLastPacketFromClientTime() < minAllowedLastPacketFromClientTime)
		{
			//can't remove from map while in iterator, so just remember for later...
			clientsToDC.push_back(pair.second);
		}
	}

	for (const ClientProxyPtr& client : clientsToDC)
	{
		HandleClientDisconnected(client);
	}
}

void NetworkManagerServer::HandleClientDisconnected(const ClientProxyPtr& inClientProxy)
{
	int player_id = inClientProxy->GetPlayerId();

	auto p_function = [this, player_id](const ClientProxyPtr& client)
	{
		NotifyPlayerQuit(client, player_id);
	};

	SendPacketToAll(p_function);

	mPlayerIdToClientMap.erase(player_id);
	mAddressToClientMap.erase(inClientProxy->GetSocketAddress());


	LOG("Server Player Left, Player %d left the lobby", player_id)

	
}

void NetworkManagerServer::NotifyPlayerQuit(const ClientProxyPtr& client, int player_id)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Quit);
	packet.Write(player_id);

	SendPacket(packet, client->GetSocketAddress());
}

void NetworkManagerServer::RegisterGameObject(const GameObjectPtr& inGameObject)
{
	//assign network id
	const int newNetworkId = GetNewNetworkId();
	inGameObject->SetNetworkId(newNetworkId);

	//add mapping from network id to game object
	m_network_id_to_game_object_map[newNetworkId] = inGameObject;

	//tell all client proxies this is new...
	for (const auto& pair : mAddressToClientMap)
	{
		pair.second->GetReplicationManagerServer().ReplicateCreate(
			newNetworkId, inGameObject->GetAllStateMask());
	}
}


void NetworkManagerServer::UnregisterGameObject(const GameObject* inGameObject)
{
	const int networkId = inGameObject->GetNetworkId();
	m_network_id_to_game_object_map.erase(networkId);

	//tell all client proxies to STOP replicating!
	for (const auto& pair : mAddressToClientMap)
	{
		pair.second->GetReplicationManagerServer().ReplicateDestroy(networkId);
	}
}

void NetworkManagerServer::SetStateDirty(const int inNetworkId, const uint32_t inDirtyState) const
{
	//tell everybody this is dirty
	for (const auto& pair : mAddressToClientMap)
	{
		pair.second->GetReplicationManagerServer().SetStateDirty(inNetworkId, inDirtyState);
	}
}
