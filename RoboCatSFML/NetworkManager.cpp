#include "ColorJumpPCH.hpp"

NetworkManager::NetworkManager() :
	m_bytes_sent_this_frame(0),
	m_drop_packet_chance(0.f),
	m_simulated_latency(0.f)
{
}

NetworkManager::~NetworkManager()
= default;

bool NetworkManager::Init(const uint16_t in_port)
{
	m_socket = SocketUtil::CreateUDPSocket(INET);
	const SocketAddress ownAddress(INADDR_ANY, in_port);
	m_socket->Bind(ownAddress);

	LOG("Initializing NetworkManager at port %d", in_port)

	m_bytes_received_per_second = WeightedTimedMovingAverage(1.f);
	m_bytes_sent_per_second = WeightedTimedMovingAverage(1.f);

	//did we bind okay?
	if (m_socket == nullptr)
	{
		return false;
	}

	if (m_socket->SetNonBlockingMode(true) != NO_ERROR)
	{
		return false;
	}

	return true;
}

void NetworkManager::ProcessIncomingPackets()
{
	ReadIncomingPacketsIntoQueue();

	ProcessQueuedPackets();

	UpdateBytesSentLastFrame();
}

void NetworkManager::ReadIncomingPacketsIntoQueue()
{
	//should we just keep a static one?
	char packetMem[1500];
	constexpr int packetSize = sizeof(packetMem);
	InputMemoryBitStream inputStream(packetMem, packetSize * 8);
	SocketAddress fromAddress;

	//keep reading until we don't have anything to read ( or we hit a max number that we'll process per frame )
	int receivedPacketCount = 0;
	int totalReadByteCount = 0;

	while (receivedPacketCount < kMaxPacketsPerFrameCount)
	{
		const int readByteCount = m_socket->ReceiveFrom(packetMem, packetSize, fromAddress);
		if (readByteCount == 0)
		{
			//nothing to read
			break;
		}
		if (readByteCount == -WSAECONNRESET)
		{
			//port closed on other end, so DC this person immediately
			HandleConnectionReset(fromAddress);
		}
		else if (readByteCount > 0)
		{
			inputStream.ResetToCapacity(readByteCount);
			++receivedPacketCount;
			totalReadByteCount += readByteCount;

			//now, should we drop the packet?
			if (RoboMath::GetRandomFloat() >= m_drop_packet_chance)
			{
				//we made it
				//shove the packet into the queue and we'll handle it as soon as we should...
				//we'll pretend it wasn't received until simulated latency from now
				//this doesn't sim jitter, for that we would need to.....

				float simulatedReceivedTime = Timing::sInstance.GetTimef() + m_simulated_latency;
				m_packet_queue.emplace(simulatedReceivedTime, inputStream, fromAddress);
			}
			else
			{
				LOG("Dropped packet!", 0)
				//dropped!
			}
		}
		else
		{
			//uhoh, error? exit or just keep going?
		}
	}

	if (totalReadByteCount > 0)
	{
		m_bytes_received_per_second.UpdatePerSecond(static_cast<float>(totalReadByteCount));
	}
}

void NetworkManager::ProcessQueuedPackets()
{
	//look at the front packet...
	while (!m_packet_queue.empty())
	{
		ReceivedPacket& nextPacket = m_packet_queue.front();
		if (Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime())
		{
			ProcessPacket(nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress());
			m_packet_queue.pop();
		}
		else
		{
			break;
		}
	}
}

void NetworkManager::SendPacket(const OutputMemoryBitStream& inOutputStream,
                                const SocketAddress& inFromAddress)
{
	const int sentByteCount = m_socket->SendTo(inOutputStream.GetBufferPtr(),
	                                           inOutputStream.GetByteLength(), inFromAddress);
	if (sentByteCount > 0)
	{
		m_bytes_sent_this_frame += sentByteCount;
	}
}

void NetworkManager::UpdateBytesSentLastFrame()
{
	if (m_bytes_sent_this_frame > 0)
	{
		m_bytes_sent_per_second.UpdatePerSecond(static_cast<float>(m_bytes_sent_this_frame));

		m_bytes_sent_this_frame = 0;
	}
}


NetworkManager::ReceivedPacket::ReceivedPacket(const float in_received_time,
                                               const InputMemoryBitStream& in_input_memory_bit_stream,
                                               const SocketAddress& in_address) :
	m_received_time(in_received_time),
	m_packet_buffer(in_input_memory_bit_stream),
	m_from_address(in_address)
{
}


void NetworkManager::AddNetworkIdToGameObjectMap(const GameObjectPtr& in_game_object)
{
	m_network_id_to_game_object_map[in_game_object->GetNetworkId()] = in_game_object;
}

void NetworkManager::RemoveNetworkIdToGameObjectMap(const GameObjectPtr& in_game_object)
{
	m_network_id_to_game_object_map.erase(in_game_object->GetNetworkId());
}
