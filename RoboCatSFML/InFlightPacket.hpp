#pragma once
class DeliveryNotificationManager;

//in case we decide to change the type of the sequence number to use fewer or more bits
using PacketSequenceNumber = uint16_t;

class InFlightPacket
{
public:
	explicit InFlightPacket(PacketSequenceNumber inSequenceNumber);

	PacketSequenceNumber GetSequenceNumber() const { return mSequenceNumber; }
	float GetTimeDispatched() const { return mTimeDispatched; }

	void SetTransmissionData(const int inKey, const TransmissionDataPtr& inTransmissionData)
	{
		mTransmissionDataMap[inKey] = inTransmissionData;
	}

	TransmissionDataPtr GetTransmissionData(const int inKey) const
	{
		const auto it = mTransmissionDataMap.find(inKey);
		return (it != mTransmissionDataMap.end()) ? it->second : nullptr;
	}

	void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
	void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;

private:
	PacketSequenceNumber mSequenceNumber;
	float mTimeDispatched;

	unordered_map<int, TransmissionDataPtr> mTransmissionDataMap;
};
