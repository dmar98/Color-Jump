#include "RoboCatServerPCH.hpp"

NetworkManagerServer* NetworkManagerServer::sInstance;


NetworkManagerServer::NetworkManagerServer() :
	mNewPlayerId(1),
	mNewNetworkId(1),
	mClientDisconnectTimeout(3.f)
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
		//need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
		//otherwise something weird is going on...
		SendWelcomePacket(inClientProxy);
		break;
	case PacketType::PT_Welcomed:
		if (inClientProxy->GetDeliveryNotificationManager().ReadAndProcessState(inInputStream))
		{
			HandleInputPacket(inClientProxy, inInputStream);
		}
		break;
	case PacketType::PT_Quit:
		HandleClientDisconnected(inClientProxy);
		break;
	case PacketType::PT_Platform: break;
	case PacketType::PT_Player: break;
	case PacketType::PT_Goal: break;
	case PacketType::PT_Team_Changed:
		HandleTeamChange(inClientProxy, inInputStream);
		break;
	case PacketType::PT_Start_Game: break;
	case PacketType::PT_Start_Game_Countdown: break;
	case PacketType::PT_Team_Death: break;
	case PacketType::PT_Checkpoint: break;
	default:
		LOG("Unknown packet type received from %s",
		    inClientProxy->GetSocketAddress().ToString().c_str())
		break;
	}
}

auto NetworkManagerServer::NotifyOfTeamChange(const ClientProxyPtr& inClientProxy)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Team_Changed);
	packet.Write(inClientProxy->GetPlayerId());
	packet.Write(inClientProxy->GetTeamID());
	packet.Write(inClientProxy->GetColor());

	LOG("Server Team Change, client '%s' as player %d changed to team %d", inClientProxy->GetName().c_str(),
		inClientProxy->GetPlayerId(), inClientProxy->GetTeamID())

		SendPacket(packet, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::HandleTeamChange(const ClientProxyPtr& inClientProxy,
                                            InputMemoryBitStream& in_input_stream)
{
	int id;
	int team_id;
	int color;
	in_input_stream.Read(id);
	in_input_stream.Read(team_id);
	in_input_stream.Read(color, 1);

	inClientProxy->SetTeamID(team_id);
	inClientProxy->SetColor(color);

	auto p_function = [this](const ClientProxyPtr& client_proxy)
	{
		NotifyOfTeamChange(client_proxy);
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
		LOG("Bad incoming packet from unknown client at socket %s",
		    inFromAddress.ToString().c_str())
	}
}

void NetworkManagerServer::SendWelcomePacket(const ClientProxyPtr& inClientProxy)
{
	OutputMemoryBitStream packet;

	packet.Write(PacketType::PT_Welcomed);
	packet.Write(inClientProxy->GetPlayerId());

	LOG("Server Welcoming, new client '%s' as player %d", inClientProxy->GetName().c_str(),
	    inClientProxy->GetPlayerId())

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

void NetworkManagerServer::RespawnCats() const
{
	for (const auto& it : mAddressToClientMap)
	{
		const ClientProxyPtr clientProxy = it.second;

		clientProxy->RespawnCatIfNecessary();
	}
}

void NetworkManagerServer::SendOutgoingPackets()
{
	//let's send a client a state packet whenever their move has come in...
	for (const auto& it : mAddressToClientMap)
	{
		const ClientProxyPtr clientProxy = it.second;
		//process any timed out packets while we're going through the list
		clientProxy->GetDeliveryNotificationManager().ProcessTimedOutPackets();

		if (clientProxy->IsLastMoveTimestampDirty())
		{
			SendStatePacketToClient(clientProxy);
		}
	}
}

void NetworkManagerServer::UpdateAllClients()
{
	for (const auto& it : mAddressToClientMap)
	{
		//process any timed out packets while we're going through the list
		it.second->GetDeliveryNotificationManager().ProcessTimedOutPackets();

		SendStatePacketToClient(it.second);
	}
}

void NetworkManagerServer::SendStatePacketToClient(const ClientProxyPtr& inClientProxy)
{
	//build state packet
	OutputMemoryBitStream statePacket;

	//it's state!
	statePacket.Write(PacketType::PT_State);

	InFlightPacket* ifp = inClientProxy->GetDeliveryNotificationManager().WriteState(statePacket);

	WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);

	AddScoreBoardStateToPacket(statePacket);

	const auto rmtd = new ReplicationManagerTransmissionData(
		&inClientProxy->GetReplicationManagerServer());
	inClientProxy->GetReplicationManagerServer().Write(statePacket, rmtd);
	ifp->SetTransmissionData('RPLM', TransmissionDataPtr(rmtd));

	SendPacket(statePacket, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream,
                                                         const ClientProxyPtr& inClientProxy)
{
	//first, dirty?
	const bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
	inOutputStream.Write(isTimestampDirty);
	if (isTimestampDirty)
	{
		inOutputStream.Write(inClientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp());
		inClientProxy->SetIsLastMoveTimestampDirty(false);
	}
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

void NetworkManagerServer::AddScoreBoardStateToPacket(OutputMemoryBitStream& inOutputStream)
{
	ScoreBoardManager::sInstance->Write(inOutputStream);
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

void NetworkManagerServer::HandleInputPacket(const ClientProxyPtr& inClientProxy,
                                             InputMemoryBitStream& inInputStream)
{
	uint32_t moveCount = 0;
	Move move;
	inInputStream.Read(moveCount, 2);

	for (; moveCount > 0; --moveCount)
	{
		if (move.Read(inInputStream))
		{
			if (inClientProxy->GetUnprocessedMoveList().AddMoveIfNew(move))
			{
				inClientProxy->SetIsLastMoveTimestampDirty(true);
			}
		}
	}
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
	mPlayerIdToClientMap.erase(inClientProxy->GetPlayerId());
	mAddressToClientMap.erase(inClientProxy->GetSocketAddress());
	dynamic_cast<Server*>(Engine::s_instance.get())->HandleLostClient(inClientProxy);

	//was that the last client? if so, bye!
	if (mAddressToClientMap.empty())
	{
		Engine::s_instance->SetShouldKeepRunning(false);
	}
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
