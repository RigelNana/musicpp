#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <ostream>
#include <string>

namespace musicpp {

struct interval {
  std::int8_t fifths{0};
  std::int8_t octaves{0};

  constexpr interval() noexcept = default;
  constexpr interval(std::int8_t f, std::int8_t o) noexcept
      : fifths(f), octaves(o) {}

  [[nodiscard]] constexpr interval
  operator()(std::int8_t oct_offset) const noexcept {
    return {fifths, static_cast<std::int8_t>(octaves + oct_offset)};
  }

  [[nodiscard]] constexpr interval
  operator-() const noexcept {
    return {static_cast<std::int8_t>(-fifths),
            static_cast<std::int8_t>(-octaves)};
  }

  [[nodiscard]] friend constexpr interval
  operator+(const interval &lhs, const interval &rhs) noexcept {
    return {static_cast<std::int8_t>(lhs.fifths + rhs.fifths),
            static_cast<std::int8_t>(lhs.octaves + rhs.octaves)};
  }

  [[nodiscard]] friend constexpr interval
  operator-(const interval &lhs, const interval &rhs) noexcept {
    return {static_cast<std::int8_t>(lhs.fifths - rhs.fifths),
            static_cast<std::int8_t>(lhs.octaves - rhs.octaves)};
  }


  constexpr auto operator<=>(const interval &) const noexcept = default;

  [[nodiscard]] constexpr std::int8_t semitones() const noexcept {
    auto s = static_cast<std::int8_t>((fifths * 7) % 12);
    return static_cast<std::int8_t>((s + 12) % 12);
  }

  [[nodiscard]] constexpr bool
  is_enharmonic(const interval &other) const noexcept {
    return semitones() == other.semitones();
  }

  [[nodiscard]] constexpr std::string str() const {
    int raw_number = fifths * 4 + octaves * 7 + 1;
    if (raw_number < 1) {
      return "-" + (-*this).str();
    }

    constexpr auto generic_map = std::to_array<int>({1, 5, 2, 6, 3, 7, 4});
    constexpr auto base_fifths_map = std::to_array<int>({0, 1, 2, 3, 4, 5, -1});

    int f = ((fifths % 7) + 7) % 7;
    int quality_offset = (fifths - base_fifths_map[f]) / 7;

    std::string result;
    bool is_perfect = (generic_map[f] == 1 || generic_map[f] == 4 || generic_map[f] == 5);
    if (is_perfect) {
      if (quality_offset == 0)      result = "P";
      else if (quality_offset == 1) result = "A";
      else if (quality_offset == -1) result = "d";
      else if (quality_offset > 1)  result = std::string(quality_offset, 'A');
      else                          result = std::string(-quality_offset, 'd');
    } else {
      if (quality_offset == 0)      result = "M";
      else if (quality_offset == -1) result = "m";
      else if (quality_offset >= 1) result = std::string(quality_offset, 'A');
      else                          result = std::string(-quality_offset - 1, 'd');
    }
    result += std::to_string(raw_number);
    return result;
  }

  interval& operator+=(const interval& other) noexcept {
    *this = *this + other;
    return *this;
  }

  friend std::ostream &operator<<(std::ostream &os, const interval &iv) {
    return os << iv.str();
  }
};
namespace intervals {
constexpr auto A1 = interval(7, -4);

constexpr auto P1 = interval(0, 0);
constexpr auto P4 = interval(-1, 1);
constexpr auto P5 = interval(1, 0);
constexpr auto P8 = interval(0, 1);

constexpr auto m2 = interval(-5, 3);
constexpr auto M2 = interval(2, -1);
constexpr auto m3 = interval(-3, 2);
constexpr auto M3 = interval(4, -2);
constexpr auto m6 = interval(-4, 3);
constexpr auto M6 = interval(3, -1);
constexpr auto m7 = interval(-2, 2);
constexpr auto M7 = interval(5, -2);

constexpr auto d2 = interval(-12, 7);
constexpr auto A2 = interval(9, -5);
constexpr auto d3 = interval(-10, 6);
constexpr auto A3 = interval(11, -6);
constexpr auto d4 = interval(-8, 5);
constexpr auto A4 = interval(6, -3);
constexpr auto d5 = interval(-6, 4);
constexpr auto A5 = interval(8, -4);
constexpr auto d7 = interval(-9, 6);
constexpr auto A7 = interval(12, -6);

constexpr auto m9 = m2(1);
constexpr auto M9 = M2(1);
constexpr auto A9 = A2(1);
constexpr auto P11 = P4(1);
constexpr auto A11 = A4(1);
constexpr auto M13 = M6(1);
constexpr auto m13 = m6(1);
}

}

template <>
struct std::formatter<musicpp::interval> : std::formatter<std::string> {
  auto format(const musicpp::interval &iv, auto &ctx) const {
    return std::formatter<std::string>::format(iv.str(), ctx);
  }
};