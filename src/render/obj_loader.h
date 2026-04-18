#pragma once
#include "mesh.h"

#include <string>

bool load_obj_file(const std::string& path, render::Mesh& out);
