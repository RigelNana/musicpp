#pragma once
#include "chord_sequence.hpp"
#include "duration.hpp"
#include "melody.hpp"
#include <cstdint>
#include <format>
#include <ostream>
#include <string>

namespace musicpp {


struct time_signature {
  std::int8_t beats{4};
  std::int8_t beat_unit{4};

  constexpr time_signature() noexcept = default;
  constexpr time_signature(int b, int u) noexcept
      : beats(static_cast<std::int8_t>(b)),
        beat_unit(static_cast<std::int8_t>(u)) {}

  [[nodiscard]] constexpr duration beat_duration() const noexcept {
    return {1, static_cast<int>(beat_unit)};
  }

  [[nodiscard]] constexpr duration bar_duration() const noexcept {
    return {static_cast<int>(beats), static_cast<int>(beat_unit)};
  }


  [[nodiscard]] constexpr bool is_simple() const noexcept {
    return beats == 2 || beats == 3 || beats == 4;
  }

  [[nodiscard]] constexpr bool is_compound() const noexcept {
    return beats >= 6 && beats % 3 == 0;
  }

  [[nodiscard]] constexpr bool is_irregular() const noexcept {
    return !is_simple() && !is_compound();
  }

  [[nodiscard]] constexpr int pulse_count() const noexcept {
    return is_compound() ? beats / 3 : static_cast<int>(beats);
  }

  [[nodiscard]] constexpr duration pulse_duration() const noexcept {
    if (is_compound())
      return duration{3, static_cast<int>(beat_unit)};
    return beat_duration();
  }


  [[nodiscard]] constexpr bool fills(duration d, int bars = 1) const noexcept {
    return d == bar_duration() * bars;
  }

  [[nodiscard]] constexpr int bar_count(duration d) const noexcept {
    auto bar = bar_duration();
    int total = d.num * bar.den;
    int per_bar = d.den * bar.num;
    if (per_bar == 0)
      return 0;
    return total / per_bar;
  }

  [[nodiscard]] constexpr duration remainder(duration d) const noexcept {
    auto bar = bar_duration();
    int whole_bars = bar_count(d);
    return d + duration{-(bar.num * whole_bars), static_cast<int>(bar.den)};
  }


  [[nodiscard]] constexpr std::string str() const {
    return std::to_string(beats) + "/" + std::to_string(beat_unit);
  }

  constexpr auto operator<=>(const time_signature &) const noexcept = default;

  friend std::ostream &operator<<(std::ostream &os,
                                  const time_signature &ts) {
    return os << ts.str();
  }
};

namespace time_signatures {
constexpr auto common      = time_signature{4, 4};
constexpr auto cut         = time_signature{2, 2};
constexpr auto waltz       = time_signature{3, 4};
constexpr auto six_eight   = time_signature{6, 8};
constexpr auto nine_eight  = time_signature{9, 8};
constexpr auto twelve_eight = time_signature{12, 8};
constexpr auto five_four   = time_signature{5, 4};
constexpr auto seven_eight = time_signature{7, 8};
}


struct tempo {
  double bpm{120.0};
  duration beat{1, 4};

  constexpr tempo() noexcept = default;
  constexpr tempo(double b) noexcept : bpm(b), beat{1, 4} {}
  constexpr tempo(double b, duration d) noexcept : bpm(b), beat(d) {}

  [[nodiscard]] constexpr double seconds_per_beat() const noexcept {
    return 60.0 / bpm;
  }

  [[nodiscard]] constexpr double seconds(duration d) const noexcept {
    double beat_count =
        static_cast<double>(d.num * beat.den) / (d.den * beat.num);
    return beat_count * 60.0 / bpm;
  }

  [[nodiscard]] constexpr double ms(duration d) const noexcept {
    return seconds(d) * 1000.0;
  }

  [[nodiscard]] constexpr double bar_seconds(time_signature ts,
                                             int bars = 1) const noexcept {
    return seconds(ts.bar_duration() * bars);
  }


  [[nodiscard]] std::string str() const {
    return std::to_string(static_cast<int>(bpm)) + " BPM (" + beat.str() +
           "=" + std::to_string(static_cast<int>(bpm)) + ")";
  }

  friend std::ostream &operator<<(std::ostream &os, const tempo &t) {
    return os << t.str();
  }
};

namespace tempos {
constexpr auto largo      = tempo{50};
constexpr auto adagio     = tempo{70};
constexpr auto andante    = tempo{92};
constexpr auto moderato   = tempo{112};
constexpr auto allegro    = tempo{140};
constexpr auto vivace     = tempo{170};
constexpr auto presto     = tempo{190};
}


struct metric_position {
  int bar{0};
  duration offset{0, 1};

  [[nodiscard]] constexpr bool is_downbeat() const noexcept {
    return offset == duration{0, 1};
  }

  [[nodiscard]] constexpr bool
  is_on_beat(time_signature ts) const noexcept {
    auto bd = ts.beat_duration();
    int numer = offset.num * bd.den;
    int denom = offset.den * bd.num;
    if (denom == 0)
      return true;
    return numer % denom == 0;
  }

  [[nodiscard]] constexpr int
  beat_index(time_signature ts) const noexcept {
    auto bd = ts.beat_duration();
    return (offset.num * bd.den) / (offset.den * bd.num);
  }

  [[nodiscard]] std::string str() const {
    return std::to_string(bar + 1) + ":" +
           std::to_string(offset.beats() + 1.0);
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const metric_position &mp) {
    return os << mp.str();
  }
};


namespace detail {
inline constexpr void advance_position(metric_position &pos, duration d,
                                       time_signature ts) noexcept {
  pos.offset = pos.offset + d;
  auto bar_dur = ts.bar_duration();
  while (pos.offset >= bar_dur) {
    pos.offset =
        pos.offset + duration{-bar_dur.num, static_cast<int>(bar_dur.den)};
    ++pos.bar;
  }
}
}


template <std::size_t N>
template <typename F>
constexpr void melody<N>::walk(time_signature ts, F &&f) const {
  metric_position pos{0, {0, 1}};
  for (const auto &ev : *this) {
    f(ev, pos);
    detail::advance_position(pos, ev.dur, ts);
  }
}


template <typename... Events>
template <typename F>
constexpr void chord_sequence<Events...>::walk(time_signature ts, F &&f) const {
  metric_position pos{0, {0, 1}};
  for_each([&](const auto &ev) {
    f(ev, pos);
    detail::advance_position(pos, ev.dur, ts);
  });
}

}


template <>
struct std::formatter<musicpp::time_signature> : std::formatter<std::string> {
  auto format(const musicpp::time_signature &ts, auto &ctx) const {
    return std::formatter<std::string>::format(ts.str(), ctx);
  }
};

template <>
struct std::formatter<musicpp::tempo> : std::formatter<std::string> {
  auto format(const musicpp::tempo &t, auto &ctx) const {
    return std::formatter<std::string>::format(t.str(), ctx);
  }
};

template <>
struct std::formatter<musicpp::metric_position>
    : std::formatter<std::string> {
  auto format(const musicpp::metric_position &mp, auto &ctx) const {
    return std::formatter<std::string>::format(mp.str(), ctx);
  }
};
