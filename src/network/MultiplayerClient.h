#pragma once

#include "NetworkSerialization.h"
#include "NetworkSocketLibrary.h"
#include "NetworkTcpSocket.h"
#include "ThreadSafeMessageQueue.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace ve::network
{
	class MultiplayerClient
	{
	public:
		/**
		 * Disconnects the client before destruction.
		 */
		~MultiplayerClient();

		/**
		 * Connects to a server and starts the receive thread.
		 *
		 * @param serverEndpoint Host and port of the server.
		 * @param playerName Name sent in the first hello packet.
		 * @return True when the socket connected and hello was sent.
		 */
		bool Connect(const NetworkEndpoint& serverEndpoint, const std::string& playerName);

		/**
		 * Closes the socket and stops the receive thread.
		 */
		void Disconnect();

		/**
		 * Sends the latest local player transform to the server.
		 *
		 * @param playerSnapshot Serialized player state fields.
		 * @return True when the message was fully sent.
		 */
		bool SendPlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot);

		/**
		 * Sends one block edit to the server.
		 *
		 * @param blockMutation Block coordinates and target block id.
		 * @return True when the message was fully sent.
		 */
		bool SendBlockMutation(const BlockMutationPayload& blockMutation);

		/**
		 * Returns received messages without blocking the game thread.
		 *
		 * @return Messages received by the worker thread.
		 */
		std::vector<NetworkMessage> DrainIncomingMessages();

		/**
		 * Reports whether a socket is currently connected.
		 *
		 * @return True while the receive thread still owns an open socket.
		 */
		bool IsConnected() const noexcept;

	private:
		bool SendMessage(NetworkMessage message);
		void ReceiveMessagesUntilDisconnected(std::stop_token stopToken, std::shared_ptr<TcpSocket> receiveSocket);

		SocketLibrary _socketLibrary;
		std::shared_ptr<TcpSocket> _connectedSocket;
		std::jthread _receiveThread;
		std::mutex _sendMutex;
		std::atomic_bool _isConnected = false;
		std::atomic_uint32_t _nextOutboundSequenceNumber = 1;
		ThreadSafeMessageQueue<NetworkMessage> _incomingMessages;
	};
}
