#pragma once
#include <string>

namespace musicpp {


struct degree {
  int num{0};
  int alter{0};

  constexpr degree() noexcept = default;
  constexpr degree(int n) noexcept : num(n), alter(0) {}
  constexpr degree(int n, int a) noexcept : num(n), alter(a) {}

  constexpr bool operator==(const degree &) const noexcept = default;

  [[nodiscard]] constexpr explicit operator bool() const noexcept {
    return num >= 1;
  }

  [[nodiscard]] std::string prefix() const {
    if (alter == -1) return "b";
    if (alter == +1) return "#";
    return "";
  }
};

[[nodiscard]] consteval degree b(int d) noexcept { return {d, -1}; }
[[nodiscard]] consteval degree s(int d) noexcept { return {d, +1}; }

}
