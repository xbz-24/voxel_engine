#include "NetworkAsioRuntime.h"

namespace ve::network
{
	/** Stops the ASIO runtime before shutdown. */
	NetworkAsioRuntime::~NetworkAsioRuntime()
	{
		Stop();
	}

	/** Returns the mutable ASIO context. */
	asio::io_context& NetworkAsioRuntime::Context() noexcept { return io_context_; }

	/** Returns the read-only ASIO context. */
	const asio::io_context& NetworkAsioRuntime::Context() const noexcept { return io_context_; }

	/** Executes all immediately ready handlers. */
	void NetworkAsioRuntime::Poll()
	{
		io_context_.poll();
	}

	/** Executes at most one ready handler. */
	std::size_t NetworkAsioRuntime::RunOne()
	{
		return io_context_.run_one();
	}

	/** Restarts the context so it can process work again. */
	void NetworkAsioRuntime::Restart()
	{
		io_context_.restart();
	}

	/** Stops the context and releases pending async waits. */
	void NetworkAsioRuntime::Stop()
	{
		io_context_.stop();
	}

	/** Reports whether the context is stopped. */
	bool NetworkAsioRuntime::IsStopped() const noexcept { return io_context_.stopped(); }
}
