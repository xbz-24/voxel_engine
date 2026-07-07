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
		if (!_socketLibrary.IsAvailable()) return false;
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
		_isConnected = false;
		if (_receiveThread.joinable()) _receiveThread.request_stop();
		if (_connectedSocket) _connectedSocket->Close();
		_connectedSocket.reset();
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
		while (!stopToken.stop_requested() && receiveSocket && receiveSocket->IsOpen())
		{
			std::optional<NetworkMessage> receivedMessage = ReceiveNetworkMessage(*receiveSocket);
			if (!receivedMessage) break;
			_incomingMessages.Push(std::move(*receivedMessage));
		}
		_isConnected = false;
	}
}
