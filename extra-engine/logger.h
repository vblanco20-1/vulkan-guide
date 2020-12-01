#pragma once
#include <string_view>
#include "fmt/core.h"
#include "fmt/os.h"
#include "fmt/color.h"
#include <chrono>
#include <fmt/chrono.h>

#define LOG_FATAL(message,...) LogHandler::Get().log(LogType::Fatal,message, ##__VA_ARGS__);
#define LOG_ERROR(message,...) LogHandler::Get().log(LogType::Error,message, ##__VA_ARGS__);
#define LOG_INFO(message,...) LogHandler::Get().log(LogType::Info,message, ##__VA_ARGS__);
#define LOG_WARNING(message,...) LogHandler::Get().log(LogType::Warning,message, ##__VA_ARGS__);
#define LOG_SUCCESS(message,...) LogHandler::Get().log(LogType::Success,message, ##__VA_ARGS__);

enum class LogType {
	Fatal,
	Error,
	Info,
	Warning,
	Success
};

class LogHandler {
public:
	template <typename... Args>
	inline static void print(std::string_view message, Args... args)
	{
		fmt::print((message), args...);
		fmt::print("\n");
	}

	template <typename... Args>
	inline static void log(LogType type,std::string_view message, Args... args)
	{
		print_time();
		switch (type)
		{
		case LogType::Fatal:
			fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
				"[FATAL]   ");
			break;
		case LogType::Error:
			fmt::print(fg(fmt::color::crimson),
				"[ERROR]   ");
			break;
		case LogType::Warning:
			fmt::print(fg(fmt::color::yellow),
				"[WARNING] ");
			break;
		case LogType::Success:
			fmt::print(fg(fmt::color::light_green),
				"[SUCCESS] ");
			break;
		case LogType::Info:
			fmt::print(fg(fmt::color::white),
				"[INFO]    ");
			break;
		}		
		
		print(message, args...);

		if (type == LogType::Fatal)
		{
			abort();
		}
	}


	inline static void print_time() {
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

		auto time_point = now - LogHandler::Get().start_time;

		
		fmt::print("[{:%M:%S}]", time_point);
	}

	inline static LogHandler& Get() {
		static LogHandler handler{};
		return handler;
	}

	void set_time() {
		start_time = std::chrono::system_clock::now();
	}

	std::chrono::time_point<std::chrono::system_clock> start_time;
};