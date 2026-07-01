#pragma once

#include "body.h"
#include "contact.h"

#include <optional>

namespace physics::collision {

std::optional<Contact> test(const Body& a, const Body& b);

} // namespace physics::collision
