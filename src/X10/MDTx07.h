#ifndef X10_MDTX07_H_
#define X10_MDTX07_H_

#include "BaseDevice.h"

namespace X10 {

class MDTx07: public BaseDevice {
public:
    MDTx07(Address address, string& name);
    virtual ~MDTx07();
};

} /* namespace X10 */

#endif /* X10_MDTX07_H_ */
