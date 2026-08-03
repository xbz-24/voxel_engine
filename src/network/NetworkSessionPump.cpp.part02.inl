				pumpStats.blockMutationsApplied++;
				pumpStats.messagesPublished += _server.BroadcastExcept(inboundMessage.connectionId, inboundMessage.message);
			}
			else
			{
				pumpStats.invalidMessagesRejected++;
			}
		}
		return pumpStats;
	}

	NetworkPumpStats NetworkSession::ApplyClientMessages(ve::world::World& world)
	{
		NetworkPumpStats pumpStats;
		std::size_t acceptedBlockMutationCountForClient = 0;
		for (const NetworkMessage& message : _client.DrainIncomingMessages())
		{
			pumpStats.messagesReceived++;
			const std::optional<ve::gameplay::BlockInteraction> blockInteraction = TryAcceptBlockMutationForPump(
				pumpStats,
				message,
				_serverToClientSequenceTracker,
				acceptedBlockMutationCountForClient,
				MaxInboundBlockMutationsAppliedToClientPerPump);
			if (!blockInteraction) continue;
			if (ApplyNetworkBlockMutation(world, *blockInteraction))
			{
				pumpStats.blockMutationsApplied++;
			}
			else
			{
				pumpStats.invalidMessagesRejected++;
			}
		}
		return pumpStats;
	}
}
