#pragma once
using IntToGameObjectMap = unordered_map<int, GameObjectPtr>;

class NetworkManager
{
public:
	enum class PacketType
	{
		PT_Hello,
		PT_Quit,
		PT_Platform,
		PT_Player,
		PT_Goal,
		PT_Team_Changed,
		PT_Start_Game,
		PT_Start_Game_Countdown,
		PT_Team_Death,
		PT_Checkpoint,
		PT_State_Lobby,
		PT_Player_Connect,
		PT_Initial_State,
		PT_Ready,
		PT_Spawn,
		PT_Game_State
	};

	static constexpr int kMaxPacketsPerFrameCount = 10;

	NetworkManager();
	virtual ~NetworkManager();

	bool Init(uint16_t in_port);
	void ProcessIncomingPackets();
	virtual void ProcessPacket(InputMemoryBitStream& inInputStream,
	                           const SocketAddress& inFromAddress) = 0;
	virtual void HandleConnectionReset(const SocketAddress& inFromAddress) { (void)inFromAddress; }
	void SendPacket(const OutputMemoryBitStream& inOutputStream,
	                const SocketAddress& inFromAddress);

	const WeightedTimedMovingAverage& GetBytesReceivedPerSecond() const
	{
		return m_bytes_received_per_second;
	}

	const WeightedTimedMovingAverage& GetBytesSentPerSecond() const
	{
		return m_bytes_sent_per_second;
	}

	void SetDropPacketChance(const float in_chance) { m_drop_packet_chance = in_chance; }
	float GetDropPacketChance() const { return m_drop_packet_chance; }
	void SetSimulatedLatency(const float in_latency) { m_simulated_latency = in_latency; }
	float GetSimulatedLatency() const { return m_simulated_latency; }

	inline GameObjectPtr GetGameObject(int in_network_id) const;
	void AddNetworkIdToGameObjectMap(const GameObjectPtr& in_game_object);
	void RemoveNetworkIdToGameObjectMap(const GameObjectPtr& in_game_object);

protected:
	IntToGameObjectMap m_network_id_to_game_object_map;

private:
	class ReceivedPacket
	{
	public:
		ReceivedPacket(float in_received_time,
		               const InputMemoryBitStream& in_input_memory_bit_stream,
		               const SocketAddress& in_address);
		const SocketAddress& GetFromAddress() const { return m_from_address; }
		float GetReceivedTime() const { return m_received_time; }
		InputMemoryBitStream& GetPacketBuffer() { return m_packet_buffer; }

	private:
		float m_received_time;
		InputMemoryBitStream m_packet_buffer;
		SocketAddress m_from_address;
	};

	void UpdateBytesSentLastFrame();
	void ReadIncomingPacketsIntoQueue();
	void ProcessQueuedPackets();

	queue<ReceivedPacket, list<ReceivedPacket>> m_packet_queue;

	UDPSocketPtr m_socket;

	WeightedTimedMovingAverage m_bytes_received_per_second;
	WeightedTimedMovingAverage m_bytes_sent_per_second;

	int m_bytes_sent_this_frame;
	float m_drop_packet_chance;
	float m_simulated_latency;
};

inline GameObjectPtr NetworkManager::GetGameObject(const int in_network_id) const
{
	const auto game_object_itr = m_network_id_to_game_object_map.find(in_network_id);
	if (game_object_itr != m_network_id_to_game_object_map.end())
	{
		return game_object_itr->second;
	}
	return {};
}
