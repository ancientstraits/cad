#pragma once

#include <cglm/cglm.h>
#include "object.h"

void box_create_centered(Object* o, vec3 center, vec3 dim);
void box_create(Object* o, vec3 min, vec3 dim);
