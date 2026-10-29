#include <iostream>
#include <iomanip>
#include <cmath>
#include "FrameConverter.h"
#include "Time.h"
#include "TimeConverter.h"
#include "EopData.h"

void printVector(const std::string& label, const Eigen::Vector3d& v, const std::string& units = "") {
    std::cout << std::left << std::setw(10) << label << "| "
              << std::fixed << std::setprecision(6)
              << std::setw(15) << v.x() << std::setw(15) << v.y() << std::setw(15) << v.z()
              << " " << units << std::endl;
}

void printMatrix(const Eigen::Matrix3d& m) {
    for (int i = 0; i < 3; ++i) {
        std::cout << "  | ";
        for (int j = 0; j < 3; ++j) {
            std::cout << std::setw(14) << std::fixed << std::setprecision(9) << m(i, j);
        }
        std::cout << " |" << std::endl;
    }
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "   FrameConverter: системы координат космической баллистики" << std::endl;
    std::cout << "==================================================" << std::endl;

    // === 1. Инициализация ===
    Time epochTT = Time::fromCalendar(TimeScale::TT, 2024, 9, 12, 12, 0, 0.0);

    EopData eop;
    eop.dut1 = -0.1773947;
    eop.xp   = 0.075713;
    eop.yp   = 0.282529;
    eop.dx   = 0.379e-3;
    eop.dy   = 0.007e-3;

    TimeConverter tc;
    FrameConverter<EopData, TimeConverter> fc(eop, tc);

    // === 2. Входные векторы в ITRS ===
    Eigen::Vector3d r_ITRS(6378.137, 0.0, 0.0);
    Eigen::Vector3d v_ITRS(0.0, 7.9, 0.0);

    // === 3. Преобразование ITRS -> GCRS ===
    std::cout << "\n[1] Запрос преобразования: ITRS -> GCRS" << std::endl;
    auto tr = fc(epochTT, Frame::GCRS, Frame::ITRS);

    // Вывод параметров transformation
    std::cout << "\n  Параметры transformation:" << std::endl;
    std::cout << "  --------------------------------------------" << std::endl;
    std::cout << "    Кватернион Q (w, x, y, z) : ("
              << tr.Q.w() << ", " << tr.Q.x() << ", "
              << tr.Q.y() << ", " << tr.Q.z() << ")" << std::endl;

    double angle_deg = 2.0 * std::acos(std::abs(tr.Q.w())) * 180.0 / M_PI;
    std::cout << "    Угол поворота              : " << angle_deg << "°" << std::endl;

    std::cout << "    Смещение dR                : (" 
              << tr.dR.x() << ", " << tr.dR.y() << ", " << tr.dR.z() << ") м" << std::endl;
    std::cout << "    Изменение скорости dV      : ("
              << tr.dV.x() << ", " << tr.dV.y() << ", " << tr.dV.z() << ") м/с" << std::endl;
    std::cout << "    Угловая скорость W         : ("
              << tr.W.x() << ", " << tr.W.y() << ", " << tr.W.z() << ") рад/с" << std::endl;

    // Матрица поворота
    std::cout << "\n  Матрица поворота (ITRS -> GCRS):" << std::endl;
    printMatrix(tr.Q.toRotationMatrix());

    // === 4. Применяем ===
    Eigen::Vector3d r_GCRS = tr(r_ITRS);
    auto [r_GCRS2, v_GCRS] = tr(r_ITRS, v_ITRS);

    std::cout << "\n[2] Векторы состояний:" << std::endl;
    std::cout << "  --------------------------------------------------------------------------" << std::endl;
    std::cout << "  " << std::left << std::setw(10) << "Система"
              << std::setw(17) << "X" << std::setw(15) << "Y" << std::setw(15) << "Z" << std::endl;
    std::cout << "  --------------------------------------------------------------------------" << std::endl;

    printVector("ITRS", r_ITRS, "м");
    printVector("GCRS", r_GCRS, "м");

    std::cout << "\n[3] Скорости:" << std::endl;
    std::cout << "  --------------------------------------------------------------------------" << std::endl;
    printVector("ITRS", v_ITRS, "м/с");
    printVector("GCRS", v_GCRS, "м/с");

    // === 5. Проверка обратимости ===
    auto tr_back = fc(epochTT, Frame::ITRS, Frame::GCRS);
    Eigen::Vector3d r_back = tr_back(r_GCRS);
    Eigen::Vector3d v_back = std::get<1>(tr_back(r_GCRS, v_GCRS));

    std::cout << "\n[4] Проверка обратимости (ITRS -> GCRS -> ITRS):" << std::endl;
    printVector("Исходный", r_ITRS, "м");
    printVector("Обратно", r_back, "м");
    std::cout << "  Ошибка положения: " << (r_ITRS - r_back).norm() << " м" << std::endl;
    std::cout << "  Ошибка скорости : " << (v_ITRS - v_back).norm() << " м/с" << std::endl;

    std::cout << "\n==================================================" << std::endl;
    std::cout << "  Тест завершён" << std::endl;
    std::cout << "==================================================" << std::endl;
    return 0;
}