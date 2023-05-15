#ifndef GEO_PCH_HPP
#define GEO_PCH_HPP

#include <glm/vec2.hpp>
#include <vector>
#include <utility>
#include <array>
#include <limits>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat2x2.hpp>
#include <algorithm>
#include <cstdint>
#include <cmath>
#ifdef HAS_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif
#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif
#ifdef HAS_PROFILE_TOOLS
#include "perf/perf.hpp"
#endif

#endif