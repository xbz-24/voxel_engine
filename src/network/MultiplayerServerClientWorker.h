#pragma once

#include "NetworkTcpSocket.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>

namespace ve::network
{
	struct MultiplayerServerClientWorker
	{
		std::uint32_t connectionId = 0;
		std::shared_ptr<TcpSocket> socket;
		std::uint32_t nextOutboundSequenceNumber = 1;
		std::atomic_bool finished = false;
		// Keep last so destruction joins before releasing state borrowed by the worker.
		std::jthread thread;
	};
}
