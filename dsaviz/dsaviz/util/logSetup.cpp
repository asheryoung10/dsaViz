#include <dsaviz/util/logSetup.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace dsaviz {
void setupLogging(spdlog::level::level_enum level) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "dsavizLog.txt", true);

  console_sink->set_level(level);
  file_sink->set_level(level);

  auto logger = std::make_shared<spdlog::logger>(
      "dsaviz_logger", spdlog::sinks_init_list{console_sink, file_sink});

  spdlog::set_default_logger(logger);
  spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
  spdlog::info("Logger initialized.");
}
} // namespace dsaviz