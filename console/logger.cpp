#include "logger.hpp"

void Logger::print(std::ostream& os, const std::string& log_type, const std::string& content) const
{
    os << std::format("{}[{}]: {}\033[0m", log_type, module_name, content) << std::endl;
}

Logger::Logger(const std::string& name): module_name(name)
{}