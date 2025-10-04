#include "ColliderConvex.hpp"

#include "util/common.h"

#include <ccd/ccd.h>

namespace bolt {
namespace col {

using namespace math;

inline const Vector3f* ccd2vec(const ccd_vec3_t *ccd) {
    STATIC_ASSERT(sizeof(ccd_real_t) == sizeof(float));
    STATIC_ASSERT(sizeof(ccd_vec3_t) == 3 * sizeof(float));
    return reinterpret_cast<const Vector3f*>(ccd);
}

inline Vector3f* ccd2vec(ccd_vec3_t *ccd) {
    STATIC_ASSERT(sizeof(ccd_real_t) == sizeof(float));
    STATIC_ASSERT(sizeof(ccd_vec3_t) == 3 * sizeof(float));
    return reinterpret_cast<Vector3f*>(ccd);
}

static void ccd_support(const void *obj, const ccd_vec3_t *dir, ccd_vec3_t *vec) {
    ColliderConvex* colCvx = (ColliderConvex*)obj;
    colCvx->getSupport(*ccd2vec(dir), *ccd2vec(vec));
}

static void ccd_center(const void *obj, ccd_vec3_t *center) {
    ColliderConvex* colCvx = (ColliderConvex*)obj;
    colCvx->getCenter(*ccd2vec(center));
}

bool checkConvexCollision(const ColliderConvex& c1, const ColliderConvex& c2) {
    ccd_t ccd_info;
    CCD_INIT(&ccd_info);
    ccd_info.support1 = ccd_support;
    ccd_info.support2 = ccd_support;

    ccd_info.max_iterations = 100;

    /*
    ccd_info.center1 = ccd_center;
    ccd_info.center2 = ccd_center;
    ccd_info.mpr_tolerance  = 0.01;
    */

    return ccdGJKIntersect((void*)&c1, (void*)&c2, &ccd_info) != 0;
}

} // col
} // bolt
