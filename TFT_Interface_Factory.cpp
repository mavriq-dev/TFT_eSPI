#include "TFT_Interface_Factory.h"
#include "TFT_Interface_SPI.h"
#include "TFT_Interface_Parallel.h"

namespace TFT_Runtime {

TFT_Interface* TFT_Interface_Factory::createInterface(const Config& config) {
    switch (config.interface) {
        case InterfaceMode::SPI:
            return new TFT_Interface_SPI(config);
        case InterfaceMode::PARALLEL_8BIT:
            return new TFT_Interface_Parallel(config);
        default:
            return nullptr;
    }
}

} // namespace TFT_Runtime
