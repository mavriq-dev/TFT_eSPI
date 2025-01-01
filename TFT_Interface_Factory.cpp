#include "TFT_Interface_Factory.h"
#include "TFT_Interface_SPI.h"
#include "TFT_Interface_Parallel.h"

namespace TFT_Runtime {

std::unique_ptr<TFT_Interface> TFT_Interface_Factory::createSPIInterface(const Config& config) {
    return std::make_unique<TFT_Interface_SPI>(config);
}

std::unique_ptr<TFT_Interface> TFT_Interface_Factory::createParallelInterface(const Config& config) {
    return std::make_unique<TFT_Interface_Parallel>(config);
}

} // namespace TFT_Runtime
