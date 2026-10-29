#include "EopData.h"
#include <iostream>

EopData EopData::fromFile(const std::string& path) {
    // TODO: реализовать чтение из файла IERS Bulletin A
    std::cerr << "EopData::fromFile не реализован, возвращаем нули." << std::endl;
    return EopData::zero();
}