#include "NetworkSession.h"

namespace ve::network
{
	NetworkSessionMode NetworkSession::Mode() const noexcept
	{
		return _mode;
	}

	bool NetworkSession::IsOnline() const noexcept
	{
		return _mode != NetworkSessionMode::Offline;
	}

	NetworkSessionError NetworkSession::LastError() const noexcept
	{
		return _lastError;
	}

	std::vector<NetworkSessionEvent> NetworkSession::DrainEvents()
	{
		std::vector<NetworkSessionEvent> drainedEvents;
		drainedEvents.swap(_pendingSessionEvents);
		return drainedEvents;
	}

	void NetworkSession::RecordSessionEvent(NetworkSessionEventType eventType, NetworkSessionError error)
	{
		_pendingSessionEvents.push_back(NetworkSessionEvent{ eventType, _mode, error });
	}
}
