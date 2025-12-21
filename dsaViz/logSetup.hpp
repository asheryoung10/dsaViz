#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace dsaViz {
    inline void setupLogging(spdlog::level::level_enum logLevel) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>("../logs/logs.txt", true);

        // Make sinks log everything
        console_sink->set_level(logLevel);
        file_sink->set_level(logLevel);

        // Combine sinks into a single logger
        auto logger = std::make_shared<spdlog::logger>("multi_logger", spdlog::sinks_init_list{console_sink, file_sink});

        // Set as default logger
        spdlog::set_default_logger(logger);
        spdlog::set_level(logLevel); // global log level
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
    }
}
