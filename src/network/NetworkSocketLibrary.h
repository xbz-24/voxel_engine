#pragma once

namespace ve::network
{
	class SocketLibrary
	{
	public:
		/**
		 * Starts the platform socket library for the current process.
		 */
		SocketLibrary();

		/**
		 * Stops the platform socket library when this owner goes out of scope.
		 */
		~SocketLibrary();

		/**
		 * Reports whether socket startup succeeded.
		 *
		 * @return True when socket APIs can be used.
		 */
		bool IsAvailable() const noexcept;

	private:
		bool _isStarted = false;
	};
}
