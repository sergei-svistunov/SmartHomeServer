#ifndef X10_MDTX07_H_
#define X10_MDTX07_H_

#include "Controller.h"

namespace X10 {

class MDTx07: public BaseDevice {
public:
    MDTx07(Controller& controller, Address address, string name);
    virtual ~MDTx07();
    virtual const string GetType() const {return "X10::MDTx07";}
    virtual JSON::Object GetInfo() const;
    virtual void Notify(Command command, vector<uint8_t>& data);
private:
    bool _is_on = false;
};

} /* namespace X10 */

#endif /* X10_MDTX07_H_ */
