#pragma once
#include "duration.hpp"
#include "notes.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <ostream>
#include <string>

namespace musicpp {

template <std::size_t S> struct scale_instance;
struct time_signature;
struct metric_position;


struct melody_event {
  note pitch{};
  duration dur{1, 4};
  bool is_rest{false};
  bool is_tied{false};

  [[nodiscard]] constexpr melody_event tied() const noexcept {
    return {pitch, dur, is_rest, true};
  }

  [[nodiscard]] constexpr std::string str() const {
    if (is_rest) return "-(" + dur.str() + ")";
    auto s = pitch.str() + "(" + dur.str() + ")";
    if (is_tied) s += "~";
    return s;
  }

  friend std::ostream &operator<<(std::ostream &os, const melody_event &ev) {
    return os << ev.str();
  }
};

[[nodiscard]] constexpr melody_event
operator*(const note &n, const duration &d) noexcept {
  return {n, d, false, false};
}

[[nodiscard]] constexpr melody_event rest(duration d) noexcept {
  return {{}, d, true, false};
}


template <std::size_t N> struct melody {
  std::array<melody_event, N> events;


  [[nodiscard]] static constexpr std::size_t size() noexcept { return N; }

  [[nodiscard]] constexpr const melody_event &
  operator[](std::size_t i) const noexcept {
    return events[i];
  }
  [[nodiscard]] constexpr melody_event &
  operator[](std::size_t i) noexcept {
    return events[i];
  }

  [[nodiscard]] constexpr auto begin() const noexcept { return events.begin(); }
  [[nodiscard]] constexpr auto end() const noexcept { return events.end(); }
  [[nodiscard]] constexpr auto begin() noexcept { return events.begin(); }
  [[nodiscard]] constexpr auto end() noexcept { return events.end(); }


  [[nodiscard]] constexpr melody<N + 1>
  operator|(const melody_event &ev) const noexcept {
    melody<N + 1> result{};
    std::copy(events.begin(), events.end(), result.events.begin());
    result.events[N] = ev;
    return result;
  }

  template <std::size_t M>
  [[nodiscard]] constexpr melody<N + M>
  operator|(const melody<M> &other) const noexcept {
    melody<N + M> result{};
    std::copy(events.begin(), events.end(), result.events.begin());
    std::copy(other.events.begin(), other.events.end(),
              result.events.begin() + N);
    return result;
  }


  [[nodiscard]] constexpr melody transpose(const interval &iv) const noexcept {
    melody result = *this;
    for (auto &ev : result.events)
      if (!ev.is_rest) ev.pitch = ev.pitch + iv;
    return result;
  }

  [[nodiscard]] constexpr melody retrograde() const noexcept {
    melody result = *this;
    std::ranges::reverse(result.events);
    for (auto &ev : result.events) ev.is_tied = false;
    return result;
  }

  [[nodiscard]] constexpr melody invert(const note &axis) const noexcept {
    melody result = *this;
    for (auto &ev : result.events) {
      if (!ev.is_rest) {
        auto iv = ev.pitch - axis;
        ev.pitch = axis - iv;
      }
    }
    return result;
  }

  [[nodiscard]] constexpr melody augment() const noexcept {
    melody result = *this;
    for (auto &ev : result.events)
      ev.dur = ev.dur * 2;
    return result;
  }

  [[nodiscard]] constexpr melody diminish() const noexcept {
    melody result = *this;
    for (auto &ev : result.events)
      ev.dur = duration{static_cast<int>(ev.dur.num),
                        static_cast<int>(ev.dur.den) * 2};
    return result;
  }


  [[nodiscard]] constexpr note lowest() const noexcept {
    note lo{};
    bool first = true;
    for (const auto &ev : events) {
      if (ev.is_rest) continue;
      if (first || ev.pitch.get_midi_pitch() < lo.get_midi_pitch()) {
        lo = ev.pitch;
        first = false;
      }
    }
    return lo;
  }

  [[nodiscard]] constexpr note highest() const noexcept {
    note hi{};
    bool first = true;
    for (const auto &ev : events) {
      if (ev.is_rest) continue;
      if (first || ev.pitch.get_midi_pitch() > hi.get_midi_pitch()) {
        hi = ev.pitch;
        first = false;
      }
    }
    return hi;
  }

  [[nodiscard]] constexpr interval range() const noexcept {
    return highest() - lowest();
  }

  [[nodiscard]] constexpr duration total_duration() const noexcept {
    duration sum{0, 1};
    for (const auto &ev : events)
      sum = sum + ev.dur;
    return sum;
  }

  [[nodiscard]] constexpr std::size_t note_count() const noexcept {
    std::size_t n = 0;
    for (const auto &ev : events)
      if (!ev.is_rest) ++n;
    return n;
  }

  template <std::size_t S>
  [[nodiscard]] constexpr bool
  is_diatonic(const scale_instance<S> &key) const noexcept {
    for (const auto &ev : events) {
      if (ev.is_rest) continue;
      if (!key.contains(ev.pitch)) return false;
    }
    return true;
  }


  template <std::size_t K>
  [[nodiscard]] constexpr melody<N * K> repeat() const noexcept {
    melody<N * K> result{};
    for (std::size_t k = 0; k < K; ++k)
      std::copy(events.begin(), events.end(),
                result.events.begin() + static_cast<std::ptrdiff_t>(k * N));
    return result;
  }


  template <typename F>
  constexpr void walk(time_signature ts, F &&f) const;


  [[nodiscard]] std::string str() const {
    std::string result;
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0) result += ' ';
      result += events[i].str();
    }
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os, const melody &m) {
    return os << m.str();
  }
};


[[nodiscard]] constexpr melody<2>
operator|(const melody_event &a, const melody_event &b) noexcept {
  return {{{a, b}}};
}


template <std::size_t N>
[[nodiscard]] constexpr melody<N>
operator+(const melody<N> &m, const interval &iv) noexcept {
  return m.transpose(iv);
}

template <std::size_t N>
[[nodiscard]] constexpr melody<N>
operator-(const melody<N> &m, const interval &iv) noexcept {
  return m.transpose(-iv);
}

}


template <>
struct std::formatter<musicpp::melody_event> : std::formatter<std::string> {
  auto format(const musicpp::melody_event &ev, auto &ctx) const {
    return std::formatter<std::string>::format(ev.str(), ctx);
  }
};

template <std::size_t N>
struct std::formatter<musicpp::melody<N>> : std::formatter<std::string> {
  auto format(const musicpp::melody<N> &m, auto &ctx) const {
    return std::formatter<std::string>::format(m.str(), ctx);
  }
};
