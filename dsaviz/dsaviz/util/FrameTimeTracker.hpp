#pragma once
#include <GLFW/glfw3.h>
#include <algorithm>

namespace dsaviz {

class FrameTimeTracker {
public:
  FrameTimeTracker() {}

  void initialize() {
    lastTime = glfwGetTime();
    // Initialize history to something non-zero to avoid divide-by-zero
    for (double &v : history)
      v = 1.0 / 60.0;
  }

  // Call once per frame
  void update() {
    double now = glfwGetTime();
    currentDelta = now - lastTime;
    lastTime = now;

    history[index] = currentDelta;
    index = (index + 1) % HistoryCount;
  }

  // Most recent frame time (seconds)
  double getCurrentFrameTime() const { return currentDelta; }

  // Average of last 100 frame times
  double getAverageFrameTime() const {
    double sum = 0.0;
    for (double dt : history)
      sum += dt;
    return sum / HistoryCount;
  }

  // Lowest frame time (best performance)
  double getLowFrameTime() const {
    return *std::min_element(history, history + HistoryCount);
  }

  // Highest frame time (best performance)
  double getHighFrameTime() const {
    return *std::max_element(history, history + HistoryCount);
  }

private:
  static constexpr int HistoryCount = 100;

  double history[HistoryCount] = {};
  int index = 0;

  double lastTime = 0.0;
  double currentDelta = 0.0;
};

} // namespace dsaviz