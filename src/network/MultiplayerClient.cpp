#include "MultiplayerClient.h"

#include "NetworkPacketIO.h"

namespace ve::network
{
	MultiplayerClient::~MultiplayerClient()
	{
		Disconnect();
	}

	bool MultiplayerClient::Connect(const NetworkEndpoint& serverEndpoint, const std::string& playerName)
	{
		Disconnect();
		std::optional<TcpSocket> connectedSocket = TcpSocket::Connect(serverEndpoint);
		if (!connectedSocket) return false;
		_connectedSocket = std::make_shared<TcpSocket>(std::move(*connectedSocket));
		_nextOutboundSequenceNumber = 1;
		_isConnected = true;
		const NetworkMessage helloMessage{ NetworkMessageType::ClientHello, SerializeClientHello(playerName) };
		if (!SendMessage(helloMessage))
		{
			Disconnect();
			return false;
		}
		std::shared_ptr<TcpSocket> receiveSocket = _connectedSocket;
		_receiveThread = std::jthread([this, receiveSocket](std::stop_token stopToken) { ReceiveMessagesUntilDisconnected(stopToken, receiveSocket); });
		return true;
	}

	void MultiplayerClient::Disconnect()
	{
		std::shared_ptr<TcpSocket> connected_socket = _connectedSocket;
		_isConnected = false;
		if (_receiveThread.joinable()) _receiveThread.request_stop();
		if (connected_socket) connected_socket->Shutdown();
		if (_receiveThread.joinable()) _receiveThread.join();
		{
			std::lock_guard<std::mutex> send_lock(_sendMutex);
			if (_connectedSocket == connected_socket)
			{
				if (_connectedSocket) _connectedSocket->Close();
				_connectedSocket.reset();
			}
		}
		static_cast<void>(_incomingMessages.Drain());
	}

	std::vector<NetworkMessage> MultiplayerClient::DrainIncomingMessages()
	{
		return _incomingMessages.Drain();
	}

	bool MultiplayerClient::IsConnected() const noexcept
	{
		return _isConnected;
	}

	void MultiplayerClient::ReceiveMessagesUntilDisconnected(std::stop_token stopToken, std::shared_ptr<TcpSocket> receiveSocket)
	{
		try
		{
			while (!stopToken.stop_requested() && receiveSocket)
			{
				std::optional<NetworkMessage> receivedMessage = ReceiveNetworkMessage(*receiveSocket);
				if (!receivedMessage) break;
				_incomingMessages.Push(std::move(*receivedMessage));
			}
		}
		catch (...) {}
		if (receiveSocket) receiveSocket->Shutdown();
		_isConnected = false;
	}
}
