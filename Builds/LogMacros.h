#pragma once

#define VE_LOG_TRACE(message) ::ve::log::Write(::ve::log::Level::Trace, ::ve::log::category::General, (message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_DEBUG(message) ::ve::log::Write(::ve::log::Level::Debug, ::ve::log::category::General, (message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_INFO(message) ::ve::log::Write(::ve::log::Level::Info, ::ve::log::category::General, (message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_WARNING(message) ::ve::log::Write(::ve::log::Level::Warning, ::ve::log::category::General, (message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_ERROR(message) ::ve::log::Write(::ve::log::Level::Error, ::ve::log::category::General, (message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_FATAL(message) ::ve::log::Write(::ve::log::Level::Fatal, ::ve::log::category::General, (message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_CATEGORY_INFO(category, message) ::ve::log::Write(::ve::log::Level::Info, (category), (message), { __FILE__, __FUNCTION__, __LINE__ })
#define VE_LOG_CATEGORY_ERROR(category, message) ::ve::log::Write(::ve::log::Level::Error, (category), (message), { __FILE__, __FUNCTION__, __LINE__ })
