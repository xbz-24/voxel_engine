#include <doctest/doctest.h>

#include "Logger.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{
	struct CopySensitiveCallback
	{
		std::shared_ptr<bool> reject_copies;
		std::shared_ptr<int> call_count;

		CopySensitiveCallback(std::shared_ptr<bool> reject,
			std::shared_ptr<int> calls) noexcept
			: reject_copies(std::move(reject)), call_count(std::move(calls)) {}
		CopySensitiveCallback(CopySensitiveCallback&&) noexcept = default;
		CopySensitiveCallback(const CopySensitiveCallback& other)
			: reject_copies(other.reject_copies), call_count(other.call_count)
		{
			if (*reject_copies) throw std::runtime_error("callback was copied during write");
		}
		void operator()(std::string) const { ++*call_count; }
	};
}

TEST_CASE("logger contains exceptions raised by a public callback sink")
{
	ve::log::SetConsoleEnabled(false);
	ve::log::SetCallback([](std::string) { throw std::runtime_error("callback failure"); });

	CHECK_NOTHROW(ve::log::Info("callback exceptions must not escape logging"));

	ve::log::SetCallback(std::function<void(std::string)>{});
	ve::log::SetConsoleEnabled(true);
}

TEST_CASE("logger writes without copying the user callback under its mutex")
{
	auto reject_copies = std::make_shared<bool>(false);
	auto call_count = std::make_shared<int>(0);
	CopySensitiveCallback sensitive_callback(reject_copies, call_count);
	std::function<void(std::string)> callback(std::move(sensitive_callback));
	ve::log::SetConsoleEnabled(false);
	ve::log::SetCallback(std::move(callback));
	*reject_copies = true;

	ve::log::Info("callback ownership remains stable during write");
	CHECK(*call_count == 1);

	ve::log::SetCallback(std::function<void(std::string)>{});
	ve::log::SetConsoleEnabled(true);
}
