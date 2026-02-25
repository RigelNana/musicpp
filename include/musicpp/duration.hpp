#pragma once
#include <cstdint>
#include <format>
#include <numeric>
#include <ostream>
#include <string>

namespace musicpp {


struct duration {
  std::int16_t num{1};
  std::int16_t den{4};

  constexpr duration() noexcept = default;
  constexpr duration(int n, int d) noexcept {
    if (d < 0) { n = -n; d = -d; }
    int g = std::gcd(n < 0 ? -n : n, d);
    if (g > 1) { n /= g; d /= g; }
    num = static_cast<std::int16_t>(n);
    den = static_cast<std::int16_t>(d);
  }

  [[nodiscard]] constexpr duration dotted() const noexcept {
    return {num * 3, den * 2};
  }
  [[nodiscard]] constexpr duration double_dotted() const noexcept {
    return {num * 7, den * 4};
  }
  [[nodiscard]] constexpr duration triplet() const noexcept {
    return {num * 2, den * 3};
  }

  [[nodiscard]] friend constexpr duration
  operator+(duration a, duration b) noexcept {
    return {a.num * b.den + b.num * a.den, a.den * b.den};
  }
  [[nodiscard]] friend constexpr duration
  operator*(duration d, int n) noexcept {
    return {d.num * n, static_cast<int>(d.den)};
  }
  [[nodiscard]] friend constexpr duration
  operator*(int n, duration d) noexcept {
    return d * n;
  }

  [[nodiscard]] friend constexpr bool
  operator==(duration a, duration b) noexcept {
    return static_cast<int>(a.num) * b.den ==
           static_cast<int>(b.num) * a.den;
  }
  [[nodiscard]] friend constexpr auto
  operator<=>(duration a, duration b) noexcept {
    return (static_cast<int>(a.num) * b.den) <=>
           (static_cast<int>(b.num) * a.den);
  }

  [[nodiscard]] constexpr double beats(int beat_den = 4) const noexcept {
    return static_cast<double>(num * beat_den) / den;
  }

  [[nodiscard]] constexpr std::string str() const {
    if (num == 1) {
      switch (den) {
      case 1:  return "w";
      case 2:  return "h";
      case 4:  return "q";
      case 8:  return "8th";
      case 16: return "16th";
      case 32: return "32nd";
      }
    }
    if (num == 3 && den == 2) return "w.";
    if (num == 3 && den == 4) return "h.";
    if (num == 3 && den == 8) return "q.";
    if (num == 3 && den == 16) return "8th.";
    return std::to_string(num) + "/" + std::to_string(den);
  }

  friend std::ostream &operator<<(std::ostream &os, const duration &d) {
    return os << d.str();
  }
};

namespace durations {
constexpr auto whole     = duration{1, 1};
constexpr auto half      = duration{1, 2};
constexpr auto quarter   = duration{1, 4};
constexpr auto eighth    = duration{1, 8};
constexpr auto sixteenth = duration{1, 16};
constexpr auto w = whole;
constexpr auto h = half;
constexpr auto q = quarter;
}

}

template <>
struct std::formatter<musicpp::duration> : std::formatter<std::string> {
  auto format(const musicpp::duration &d, auto &ctx) const {
    return std::formatter<std::string>::format(d.str(), ctx);
  }
};
