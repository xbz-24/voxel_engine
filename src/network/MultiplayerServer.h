#pragma once

#include "MultiplayerServerClientWorker.h"
#include "ThreadSafeMessageQueue.h"

#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace ve::network
{
	struct MultiplayerInboundMessage
	{
		std::uint32_t connectionId;
		NetworkMessage message;
	};

	struct MultiplayerServerStats
	{
		std::size_t activeClientCount = 0;
		std::size_t retainedClientWorkerCount = 0;
	};

	class MultiplayerServer
	{
	public:
		/**
		 * Stops all worker threads before destruction.
		 */
		~MultiplayerServer();

		/**
		 * Starts listening for TCP clients on a worker thread.
		 *
		 * @param listenSettings Endpoint and backlog for the listening socket.
		 * @param maxConnectedClients Maximum open client sockets accepted at once.
		 * @return True when the server socket was opened.
		 */
		bool Start(const TcpListenSettings& listenSettings, std::size_t maxConnectedClients);

		/**
		 * Closes all sockets and stops all worker threads.
		 */
		void Stop();

		/**
		 * Sends one protocol message to every connected client.
		 *
		 * @param message Message to send to every open client socket.
		 * @return Number of connected clients that accepted the write.
		 */
		std::size_t Broadcast(const NetworkMessage& message);

		/**
		 * Sends one protocol message to every connected client except one origin.
		 *
		 * @param excludedConnectionId Client connection id that should not receive the message.
		 * @param message Message to send to every other open client socket.
		 * @return Number of connected clients that accepted the write.
		 */
		std::size_t BroadcastExcept(std::uint32_t excludedConnectionId, const NetworkMessage& message);

		/**
		 * Returns received client messages without blocking the game thread.
		 *
		 * @return Messages received by worker threads.
		 */
		std::vector<MultiplayerInboundMessage> DrainIncomingMessages();

		/** @return A synchronized snapshot of active and retained client workers. */
		[[nodiscard]] MultiplayerServerStats Stats() const;

	private:
		void AcceptClientsUntilStopped(std::stop_token stopToken);
		void ReceiveClientMessages(
			std::stop_token stopToken, MultiplayerServerClientWorker& clientWorker);
		void ReapFinishedClientWorkers();

		std::shared_ptr<TcpSocket> _listeningSocket;
		std::vector<std::unique_ptr<MultiplayerServerClientWorker>> _clientWorkers;
		std::jthread _acceptThread;
		mutable std::mutex _clientsMutex;
		std::atomic_uint32_t _nextConnectionId = 1;
		std::size_t _maxConnectedClients = 8;
		std::stop_source _lifecycleStopSource;
		ThreadSafeMessageQueue<MultiplayerInboundMessage> _incomingMessages;
	};
}
