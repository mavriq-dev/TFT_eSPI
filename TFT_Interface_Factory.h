#ifndef _TFT_INTERFACE_FACTORY_H_
#define _TFT_INTERFACE_FACTORY_H_

#include "TFT_Interface.h"

namespace TFT_Runtime {

class TFT_Interface_Factory {
public:
    static TFT_Interface* createInterface(const Config& config);
};

} // namespace TFT_Runtime

#endif // _TFT_INTERFACE_FACTORY_H_
