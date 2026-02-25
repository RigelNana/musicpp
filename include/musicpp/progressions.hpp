#pragma once
#include "chord_sequence.hpp"
#include "degree.hpp"
#include "scales.hpp"
#include <string>
#include <tuple>

namespace musicpp {


template <degree D, std::size_t Tones> struct progression_step {
  chord_pattern<Tones> pattern;
  duration dur{1, 4};
  note root{};
  bool is_rest{false};
};

template <degree D, std::size_t Tones>
[[nodiscard]] constexpr auto step(const chord_pattern<Tones> &pat,
                                  duration d = {1, 4}) noexcept {
  static_assert(D.num >= 1, "Scale degree must be >= 1");
  return progression_step<D, Tones>{pat, d, {}, false};
}

template <note Root, std::size_t Tones>
[[nodiscard]] constexpr auto nominal(const chord_pattern<Tones> &pat,
                                     duration d = {1, 4}) noexcept {
  return progression_step<degree{}, Tones>{pat, d, Root, false};
}

[[nodiscard]] inline constexpr auto prog_rest(duration d) noexcept {
  return progression_step<degree{}, 1>{{{}}, d, {}, true};
}


namespace detail {
template <degree D, std::size_t Tones, std::size_t S>
[[nodiscard]] constexpr auto
realize_step(const progression_step<D, Tones> &s,
             const scale_instance<S> &key) noexcept {
  if constexpr (D.num == 0) {
    if (s.is_rest)
      return chord_event<Tones>{{}, s.dur, true, false};
    return chord_event<Tones>{s.root + s.pattern, s.dur, false, false};
  } else {
    static_assert(static_cast<std::size_t>(D.num) <= S,
                  "Scale degree exceeds scale size");
    auto root_note = key[D.num - 1];
    if constexpr (D.alter != 0) {
      root_note = root_note + interval(
          static_cast<std::int8_t>(D.alter * 7),
          static_cast<std::int8_t>(D.alter * -4));
    }
    return chord_event<Tones>{root_note + s.pattern, s.dur, false, false};
  }
}
}


template <typename... Steps> struct progression {
  std::tuple<Steps...> steps;
  static constexpr std::size_t length = sizeof...(Steps);

  template <degree D, std::size_t Tones>
  [[nodiscard]] constexpr auto
  operator|(progression_step<D, Tones> next) const noexcept {
    return std::apply(
        [&](const auto &...s) {
          return progression<Steps..., progression_step<D, Tones>>{
              std::tuple{s..., next}};
        },
        steps);
  }

  template <typename... Other>
  [[nodiscard]] constexpr auto
  operator|(const progression<Other...> &other) const noexcept {
    return std::apply(
        [&](const auto &...a) {
          return std::apply(
              [&](const auto &...b) {
                return progression<Steps..., Other...>{
                    std::tuple{a..., b...}};
              },
              other.steps);
        },
        steps);
  }

  template <std::size_t S>
  [[nodiscard]] constexpr auto
  realize(const scale_instance<S> &key) const noexcept {
    return std::apply(
        [&](const auto &...s) {
          using seq_t = chord_sequence<
              decltype(detail::realize_step(s, key))...>;
          return seq_t{std::tuple{detail::realize_step(s, key)...}};
        },
        steps);
  }

  template <std::size_t S>
  [[nodiscard]] std::string str(const scale_instance<S> &key) const {
    return realize(key).names();
  }

  template <std::size_t S>
  [[nodiscard]] std::string roman(const scale_instance<S> &key) const {
    return realize(key).roman(key);
  }
};

template <degree DA, std::size_t TA, degree DB, std::size_t TB>
[[nodiscard]] constexpr auto
operator|(progression_step<DA, TA> a, progression_step<DB, TB> b) noexcept {
  return progression<progression_step<DA, TA>, progression_step<DB, TB>>{
      std::tuple{a, b}};
}

}
