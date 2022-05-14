#pragma once
class DeliveryNotificationManager
{
public:
	DeliveryNotificationManager(bool inShouldSendAcks, bool inShouldProcessAcks);
	~DeliveryNotificationManager();

	void ProcessTimedOutPackets();

	uint32_t GetDroppedPacketCount() const { return mDroppedPacketCount; }
	uint32_t GetDeliveredPacketCount() const { return mDeliveredPacketCount; }
	uint32_t GetDispatchedPacketCount() const { return mDispatchedPacketCount; }

	const deque<InFlightPacket>& GetInFlightPackets() const { return mInFlightPackets; }

private:
	InFlightPacket* WriteSequenceNumber(OutputMemoryBitStream& inOutputStream);

	
	void HandlePacketDeliveryFailure(const InFlightPacket& inFlightPacket);
	void HandlePacketDeliverySuccess(const InFlightPacket& inFlightPacket);

	PacketSequenceNumber mNextOutgoingSequenceNumber;

	deque<InFlightPacket> mInFlightPackets;
	
	bool mShouldProcessAcks;

	uint32_t mDeliveredPacketCount;
	uint32_t mDroppedPacketCount;
	uint32_t mDispatchedPacketCount;
};