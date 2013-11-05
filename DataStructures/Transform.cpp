#include "Transform.h"

Transform::Transform() {
    position = Eigen::Vector3d(0,0,0);
    rotation = Eigen::Vector3d(0,0,0);
    scaling = Eigen::Vector3d(1,1,1);
}
