#include "ColorJumpPCH.hpp"

namespace
{
	const float kDelayBeforeAckTimeout = 0.5f;
}

DeliveryNotificationManager::DeliveryNotificationManager(bool inShouldSendAcks, bool inShouldProcessAcks) :
	mNextOutgoingSequenceNumber(0),
	mShouldProcessAcks(inShouldProcessAcks),
	mDeliveredPacketCount(0),
	mDroppedPacketCount(0),
	mDispatchedPacketCount(0)
{
}


//we're going away- log how well we did...
DeliveryNotificationManager::~DeliveryNotificationManager()
{
	if (mDispatchedPacketCount == 0)
	{
		return;
	}
	LOG("DNM destructor. Delivery rate %d%%, Drop rate %d%%",
		(100 * mDeliveredPacketCount) / mDispatchedPacketCount,
		(100 * mDroppedPacketCount) / mDispatchedPacketCount);
}

void DeliveryNotificationManager::ProcessTimedOutPackets()
{
	float timeoutTime = Timing::sInstance.GetTimef() - kDelayBeforeAckTimeout;

	while (!mInFlightPackets.empty())
	{
		const auto& nextInFlightPacket = mInFlightPackets.front();

		//was this packet dispatched before the current time minus the timeout duration?
		if (nextInFlightPacket.GetTimeDispatched() < timeoutTime)
		{
			//it failed! let us know about that
			HandlePacketDeliveryFailure(nextInFlightPacket);
			mInFlightPackets.pop_front();
		}
		else
		{
			//it wasn't, and packets are all in order by time here, so we know we don't have to check farther
			break;
		}
	}
}

void DeliveryNotificationManager::HandlePacketDeliveryFailure(const InFlightPacket& inFlightPacket)
{
	++mDroppedPacketCount;
	inFlightPacket.HandleDeliveryFailure(this);

}


void DeliveryNotificationManager::HandlePacketDeliverySuccess(const InFlightPacket& inFlightPacket)
{
	++mDeliveredPacketCount;
	inFlightPacket.HandleDeliverySuccess(this);
}
