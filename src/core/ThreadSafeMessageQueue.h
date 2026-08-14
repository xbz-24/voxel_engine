#pragma once

#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>

namespace ve::network
{
	template <typename Message>
	class ThreadSafeMessageQueue
	{
	public:
		/**
		 * Pushes one message from any worker thread.
		 *
		 * @param message Message moved into the queue.
		 */
		void Push(Message message)
		{
			std::lock_guard<std::mutex> queueLock(_messageMutex);
			_messages.push_back(std::move(message));
		}

		/**
		 * Pushes one message only when the synchronized queue has capacity.
		 *
		 * @param message Message moved into the queue when capacity remains.
		 * @param maximumMessageCount Maximum messages retained by the queue.
		 * @return True when the message was accepted; false when the queue was full.
		 */
		[[nodiscard]] bool TryPush(Message message, std::size_t maximumMessageCount)
		{
			std::lock_guard<std::mutex> queueLock(_messageMutex);
			if (_messages.size() >= maximumMessageCount) return false;
			_messages.push_back(std::move(message));
			return true;
		}

		/**
		 * Moves all currently queued messages to the caller.
		 *
		 * @return Messages accumulated since the previous drain.
		 */
		std::vector<Message> Drain()
		{
			std::lock_guard<std::mutex> queueLock(_messageMutex);
			std::vector<Message> drainedMessages;
			drainedMessages.swap(_messages);
			return drainedMessages;
		}

	private:
		std::mutex _messageMutex;
		std::vector<Message> _messages;
	};
}
