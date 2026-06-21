#pragma once

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
