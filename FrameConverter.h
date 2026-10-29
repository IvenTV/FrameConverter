#pragma once
#include "Frame.h"
#include "Transformation.h"
#include "Time.h"
#include "TimeConverter.h"
#include "EopData.h"
#include "sofa.h"
#include <Eigen/Dense>

template<typename EOP, typename TimeConv>
class FrameConverter {
    EOP _eop;
    TimeConv _tc;

public:
    FrameConverter(const EOP& eop, const TimeConv& tc) : _eop(eop), _tc(tc) {}

    transformation operator()(const Time& t, Frame to, Frame from) const {
        if (to == from) return transformation();

        // Приводим время к нужным шкалам
        TimeConverter conv;
        conv.setTime(t);
        TimeJD tt  = conv.getTime(TimeScale::TT);
        TimeJD ut1 = conv.getTime(TimeScale::UT1);

        double jdTT_int  = tt.jdInt,  jdTT_frac  = tt.jdFrac;
        double jdUT1_int = ut1.jdInt, jdUT1_frac = ut1.jdFrac;

        // --- Матрица Q (GCRS -> CIRS) ---
        double x, y, s;
        iauXy06(jdTT_int, jdTT_frac, &x, &y);
        x += _eop.dx; y += _eop.dy;
        s = iauS06(jdTT_int, jdTT_frac, x, y);
        double rc2i[3][3];
        iauC2ixys(x, y, s, rc2i);
        Eigen::Matrix3d Q_mat;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                Q_mat(i, j) = rc2i[i][j];

        // --- Матрица R (CIRS -> TIRS) ---
        double era = iauEra00(jdUT1_int, jdUT1_frac);
        double r_era[3][3];
        iauIr(r_era);
        iauRz(-era, r_era);
        Eigen::Matrix3d R_mat;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                R_mat(i, j) = r_era[i][j];

        // --- Матрица W (TIRS -> ITRS) ---
        double sp = iauSp00(jdTT_int, jdTT_frac);
        double rpom[3][3];
        iauPom00(_eop.xp, _eop.yp, sp, rpom);
        Eigen::Matrix3d W_mat;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                W_mat(i, j) = rpom[i][j];

        // Матрица из GCRS в каждую систему
        auto matFromGCRS = [&](Frame f) -> Eigen::Matrix3d {
            switch (f) {
                case Frame::GCRS: return Eigen::Matrix3d::Identity();
                case Frame::CIRS: return Q_mat;
                case Frame::TIRS: return R_mat * Q_mat;
                case Frame::ITRS: return W_mat * R_mat * Q_mat;
            }
            return Eigen::Matrix3d::Identity();
        };

        // from -> GCRS -> to
        Eigen::Matrix3d M_to   = matFromGCRS(to);
        Eigen::Matrix3d M_from = matFromGCRS(from).transpose();
        Eigen::Matrix3d final_rot = M_to * M_from;

        transformation tr;
        tr.Q  = Eigen::Quaterniond(final_rot);
        tr.dR = Eigen::Vector3d::Zero();
        tr.dV = Eigen::Vector3d::Zero();
        tr.W  = Eigen::Vector3d::Zero();
        return tr;
    }

    const EOP& eop() const { return _eop; }
};