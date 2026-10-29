#include "Frame.h"

std::optional<Frame> frameFromString(const std::string& name) {
    if (name == "GCRS") return Frame::GCRS;
    if (name == "CIRS") return Frame::CIRS;
    if (name == "TIRS") return Frame::TIRS;
    if (name == "ITRS") return Frame::ITRS;
    return std::nullopt;
}

std::string frameToString(Frame f) {
    switch (f) {
        case Frame::GCRS: return "GCRS";
        case Frame::CIRS: return "CIRS";
        case Frame::TIRS: return "TIRS";
        case Frame::ITRS: return "ITRS";
    }
    return "???";
}