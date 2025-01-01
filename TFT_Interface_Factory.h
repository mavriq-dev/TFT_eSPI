#ifndef _TFT_INTERFACE_FACTORY_H_
#define _TFT_INTERFACE_FACTORY_H_

#include "TFT_Interface.h"
#include "TFT_Runtime.h"
#include <memory>

namespace TFT_Runtime {

class TFT_Interface_Factory {
public:
    static TFT_Interface_Factory& getInstance() {
        static TFT_Interface_Factory instance;
        return instance;
    }

    std::unique_ptr<TFT_Interface> createInterface(const Config& config) {
        switch (config.interface) {
            case InterfaceMode::SPI:
                return createSPIInterface(config);
            case InterfaceMode::PARALLEL_8BIT:
                return createParallelInterface(config);
            default:
                return nullptr;
        }
    }

private:
    TFT_Interface_Factory() = default;
    ~TFT_Interface_Factory() = default;
    TFT_Interface_Factory(const TFT_Interface_Factory&) = delete;
    TFT_Interface_Factory& operator=(const TFT_Interface_Factory&) = delete;

    std::unique_ptr<TFT_Interface> createSPIInterface(const Config& config);
    std::unique_ptr<TFT_Interface> createParallelInterface(const Config& config);
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_FACTORY_H_
