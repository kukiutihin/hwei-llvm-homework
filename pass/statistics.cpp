#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <print>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using Uses = std::vector<std::pair<std::string, std::string>>;
using SequenceFreq = std::unordered_map<Uses, size_t>;
using UsesMap =
    std::unordered_map<std::string, std::unordered_set<std::string>>;

static const std::string_view USAGE = "statistics LOG_PATH {WINDOW_SIZE }+";
static const std::size_t TOP = 5;

namespace std {
template <>
struct hash<Uses> {
  std::size_t operator()(const Uses& uses) const {
    std::hash<std::string> hasher;
    std::size_t seed = uses.size();
    for (const auto& [user, used] : uses) {
      seed ^= hasher(user) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= hasher(used) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};
}  // namespace std

SequenceFreq skib(const Uses& uses, std::size_t window) {
  SequenceFreq result;
  for (std::size_t current = 0;; current++) {
    if (current + window > uses.size()) break;
    Uses localUses;
    for (std::size_t i = current; i < current + window; i++)
      localUses.push_back(uses[i]);
    result[localUses]++;
  }
  return result;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << USAGE;
    return 1;
  }

  const std::string path(argv[1]);
  std::set<size_t> windows;
  for (std::size_t i = 2; i < argc; i++) windows.insert(std::stoul(argv[i]));

  std::ifstream logFile(path);
  if (!logFile) {
    std::cerr << "Failed to open: " << path;
    return 1;
  }

  std::string line;
  Uses uses;

  while (std::getline(logFile, line)) {
    if (size_t delimIdx = line.find('<'); delimIdx != std::string::npos) {
      if (line.size() < delimIdx + 3) continue;
      auto user = line.substr(0, delimIdx - 1);
      auto used = line.substr(delimIdx + 3, line.size());
      uses.push_back({std::move(user), std::move(used)});
    }
  }

  std::stringstream buffer;

  for (const auto& window : windows) {
    auto paths = skib(uses, window);

    std::vector<std::pair<Uses, std::size_t>> ps;
    for (auto&& [list, count] : paths) ps.push_back({list, count});
    std::sort(ps.begin(), ps.end(), [](const auto& p1, const auto& p2) -> bool {
      return p1.second > p2.second;
    });

    size_t top = std::min(TOP, ps.size());
    if (top == 0) continue;
    buffer << top << " most frequent instruction subsequent with window size "
           << window << ":\n";

    for (std::size_t i = 0; i < top; i++) {
      const auto& [lines, freq] = ps[i];
      buffer << "\t" << freq << ":\n";
      for (const auto& [user, used] : lines)
        buffer << "\t\t" << user << " <- " << used << "\n";
      buffer << "\n";
    }
    buffer << "\n";
  }
  std::println("{}", buffer.str());
}
