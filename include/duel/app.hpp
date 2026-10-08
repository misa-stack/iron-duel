#pragma once
#include "duel/core.hpp"
namespace duel {
struct Options {
    Config config;
    bool headless = false;
    std::string recordPath, replayPath, verifyPath, screenshotPath, captureDirectory, smokeInputPath;
    int smokeFrames = 0;
};
int runFrontend(const Options& options);
}
