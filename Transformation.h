#pragma once
#include <Eigen/Dense>
#include <utility>

using namespace Eigen;

struct transformation {
    Vector3d dR = Vector3d::Zero();
    Vector3d dV = Vector3d::Zero();
    Quaterniond Q = Quaterniond::Identity();
    Vector3d W = Vector3d::Zero();

    Vector3d operator()(const Vector3d& r) const {
        return Q * r + dR;
    }

    std::pair<Vector3d, Vector3d> operator()(const Vector3d& r, const Vector3d& v) const {
        Vector3d r_out = Q * r + dR;
        Vector3d v_out = Q * v + dV + W.cross(Q * r);
        return {r_out, v_out};
    }

    transformation& operator*=(const transformation& other) {
        // r -> other -> this
        // 1. Смещение
        dR = Q * other.dR + dR;
        // 2. Скорость (упрощённо)
        dV = Q * other.dV + dV + W.cross(Q * other.dR);
        // 3. Вращение
        Q = Q * other.Q;
        // 4. Угловая скорость (упрощённо)
        W = W + Q * other.W;
        return *this;
    }

    transformation operator*(const transformation& other) const {
        auto cpy = *this;
        cpy *= other;
        return cpy;
    }

    void inverse() {
        Q = Q.conjugate();
        dR = -(Q * dR);
        dV = -(Q * (dV + W.cross(dR)));  // Уточнённая формула
        W = -(Q * W);
    }

    transformation inversed() const {
        auto cpy = *this;
        cpy.inverse();
        return cpy;
    }
};