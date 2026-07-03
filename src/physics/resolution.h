#pragma once
#include "body.h"
#include "contact.h"

namespace physics::resolution {

void resolve(Body& a, Body& b, const Contact& contact);

} // namespace physics::resolution
