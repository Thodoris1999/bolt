#pragma once

#include "Vector.hpp"

namespace bolt {
namespace math {

struct Matrix33f {
    float data[3][3];

    // immutable element accessor
    const float operator()(int x, int y) const { return data[x][y]; }
    // mutable element accessor
    float& operator()(int x, int y) { return data[x][y]; }

    void setZero() {
        data[0][0] = data[0][1] = data[0][2] = 0;
        data[1][0] = data[1][1] = data[1][2] = 0;
        data[2][0] = data[2][1] = data[2][2] = 0;
    }
    Matrix33f transpose() const {
        Matrix33f ret;
        ret(0,0) = data[0][0];
        ret(0,1) = data[1][0];
        ret(0,2) = data[2][0];
        ret(1,0) = data[0][1];
        ret(1,1) = data[1][1];
        ret(1,2) = data[2][1];
        ret(2,0) = data[0][2];
        ret(2,1) = data[1][2];
        ret(2,2) = data[2][2];
        return ret;
    }
    void setRotation(float roll, float pitch, float yaw);
    Vector3f operator*(const Vector3f& other) const;
};


struct Matrix34f {
    float data[3][4];

    // immutable element accessor
    const float operator()(int x, int y) const { return data[x][y]; }
    // mutable element accessor
    float& operator()(int x, int y) { return data[x][y]; }
    
    void setZero() {
        data[0][0] = data[0][1] = data[0][2] = data[0][3] = 0;
        data[1][0] = data[1][1] = data[1][2] = data[1][3] = 0;
        data[2][0] = data[2][1] = data[2][2] = data[2][3] = 0;
    }
    void setTranslation(float x, float y, float z) {
        data[0][3] = x;
        data[1][3] = y;
        data[2][3] = z;
    }
    void setRotation(float roll, float pitch, float yaw);
    Vector3f operator*(const Vector3f& other) const;
};

struct Matrix44f {
    float data[4][4];

    /// @beginGetters
    /// immutable element accessor
    const float operator()(int x, int y) const { return data[x][y]; }
    /// mutable element accessor
    float& operator()(int x, int y) { return data[x][y]; }
    Vector3f getTranslation() const { Vector3f ret(data[0][3], data[1][3], data[2][3]); return ret; }
    Matrix33f getRotation() const {
        Matrix33f ret;
        ret(0,0) = data[0][0];
        ret(0,1) = data[0][1];
        ret(0,2) = data[0][2];
        ret(1,0) = data[1][0];
        ret(1,1) = data[1][1];
        ret(1,2) = data[1][2];
        ret(2,0) = data[2][0];
        ret(2,1) = data[2][1];
        ret(2,2) = data[2][2];
        return ret;
    }
    /// @endGetters
    
    /// @beginSetters
    void setZero() {
        data[0][0] = data[0][1] = data[0][2] = data[0][3] = 0;
        data[1][0] = data[1][1] = data[1][2] = data[1][3] = 0;
        data[2][0] = data[2][1] = data[2][2] = data[2][3] = 0;
        data[3][0] = data[3][1] = data[3][2] = data[3][3] = 0;
    }
    void setIdentity() {
        setZero();
        data[0][0] = 1;
        data[1][1] = 1;
        data[2][2] = 1;
        data[3][3] = 1;
    }
    void setTranslation(float x, float y, float z) {
        data[0][3] = x;
        data[1][3] = y;
        data[2][3] = z;
    }
    void setTranslation(const Vector3f v) {
        data[0][3] = v.x;
        data[1][3] = v.y;
        data[2][3] = v.z;
    }
    void setRotation(float roll, float pitch, float yaw);
    void setRotation(const Matrix33f& mtx) {
        data[0][0] = mtx(0,0);
        data[0][1] = mtx(0,1);
        data[0][2] = mtx(0,2);
        data[1][0] = mtx(1,0);
        data[1][1] = mtx(1,1);
        data[1][2] = mtx(1,2);
        data[2][0] = mtx(2,0);
        data[2][1] = mtx(2,1);
        data[2][2] = mtx(2,2);
    }
    void makeHomogeneous() {
        data[3][0] = 0.0;
        data[3][1] = 0.0;
        data[3][2] = 0.0;
        data[3][3] = 1.0;
    }
    void setPerspective(float fovy, float aspect, float zNear, float zFar);
    /// Set to transform that looks at focus
    void setLookAt(const Vector3f& pos, const Vector3f& focus, const Vector3f& up);
    /// Set to view matrix with specified lookAt
    void setLookAtView(const Vector3f& pos, const Vector3f& focus, const Vector3f& up);
    /// @endSetters

    Matrix44f operator*(const Matrix44f& other) const;
    Vector4f operator*(const Vector4f& other) const;
    /// If this matrix represents a rigid body transform in homogeneous coordinates, return its inverse
    Matrix44f rigidInverse() const;

    static const Matrix44f IDENTITY;
};

} // math
} // bolt
