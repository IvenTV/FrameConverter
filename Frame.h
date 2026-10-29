#pragma once
#include <string>
#include <optional>

enum class Frame {
    GCRS, CIRS, TIRS, ITRS
};

std::optional<Frame> frameFromString(const std::string& name);
std::string frameToString(Frame f);