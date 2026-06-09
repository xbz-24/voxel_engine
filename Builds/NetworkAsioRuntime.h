#pragma once

#include <asio/io_context.hpp>

#include <cstddef>

namespace ve::network
{
	/** Wraps ASIO's io_context behind the engine networking namespace. */
	class NetworkAsioRuntime
	{
	public:
		/** Creates an idle ASIO runtime owned by the networking module. */
		NetworkAsioRuntime() = default;

		/** Stops pending async work before destroying the runtime. */
		~NetworkAsioRuntime();

		/** @return Mutable ASIO context used by socket adapters. */
		[[nodiscard]] asio::io_context& Context() noexcept;

		/** @return Read-only ASIO context used by diagnostics. */
		[[nodiscard]] const asio::io_context& Context() const noexcept;

		/** Runs ready handlers without blocking the caller. */
		void Poll();

		/** @return Number of handlers executed before returning. */
		[[nodiscard]] std::size_t RunOne();

		/** Restarts the context after Stop was called. */
		void Restart();

		/** Requests every pending async operation to stop. */
		void Stop();

		/** @return True when the context has been stopped. */
		[[nodiscard]] bool IsStopped() const noexcept;

	private:
		asio::io_context io_context_;
	};
}
