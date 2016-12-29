#include "gear.h"

namespace gear {
    unsigned TestType::s_instanceid = 0;

    gear::TestType throw_if_true(bool b) {
        if (b)
            throw gear::TestException("b was true");
        return {};
    }
}
