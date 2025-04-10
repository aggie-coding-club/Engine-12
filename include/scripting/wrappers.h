#pragma once
#include <glm/glm.hpp>

// Default constructor
static void ConstructVec3(void* mem) {
    new(mem) glm::vec3();
}

static void CopyConstructVec3(void* mem, const glm::vec3& other) {
    new(mem) glm::vec3(other);
}

static void ConstructVec3Float3(void* mem, float x, float y, float z) {
    new(mem) glm::vec3(x, y, z);
}

template<class A, class B>
static B* refCast(A* a)
{
    // If the handle already is a null handle, then just return the null handle
    if( !a ) return 0;

    // Now try to dynamically cast the pointer to the wanted type
    B* b = dynamic_cast<B*>(a);

    return b;
}

