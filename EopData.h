#pragma once
#include <string>

// Earth Orientation Parameters (EOP)

struct EopData{
    double dut1 = 0.0; // ΔUT1 = UT1 - UTC, секунды
    double xp   = 0.0; // Координата полюса x, радианы
    double yp   = 0.0; // Координата полюса y, радианы
    double dx   = 0.0; // Поправка δX к координате CIP X, радианы
    double dy   = 0.0; // Поправка δY к координате CIP Y, радианы

    //Заглушка: загрузка из файла IERS (реализуется позже)
    static EopData fromFile(const std::string& path);

    static EopData zero() { return {}; }
};
