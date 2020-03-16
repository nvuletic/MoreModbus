//
// Created by Nexyy on 16/03/2020.
//

#include "../src/Group.h"
#include "../src/utility/ConsoleLogger.h"

int main(int argc, char** argv)
{
    // Setup logger
    auto logger = std::unique_ptr<wolkabout::ConsoleLogger>(new wolkabout::ConsoleLogger());
    logger->setLogLevel(wolkabout::LogLevel::DEBUG);
    wolkabout::Logger::setInstance(std::move(logger));

    const auto& mapping = std::make_shared<wolkabout::Mapping>("RM1",
            wolkabout::Mapping::RegisterType::HOLDING_REGISTER, 0, wolkabout::Mapping::OperationType::TAKE_BIT,
            static_cast<uint8_t>(0));

    const auto& group = std::make_shared<wolkabout::Group>(mapping);

    LOG(DEBUG) << "Aloha!";

    return 0;
}
