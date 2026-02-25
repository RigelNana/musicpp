#pragma once
#include "chords.hpp"
#include "duration.hpp"
#include <format>
#include <ostream>
#include <string>
#include <tuple>

namespace musicpp {

struct time_signature;
struct metric_position;


template <std::size_t N> struct chord_event {
  chord_instance<N> chord;
  duration dur{1, 4};
  bool is_rest{false};
  bool is_tied{false};

  [[nodiscard]] constexpr chord_event tied() const noexcept {
    return {chord, dur, is_rest, true};
  }


  [[nodiscard]] auto analyze() const { return chord.analyze(); }

  [[nodiscard]] auto analyze(const note &root) const {
    return chord.analyze(root);
  }

  template <std::size_t S>
  [[nodiscard]] auto analyze(const scale_instance<S> &key) const {
    return chord.analyze(key);
  }

  template <std::size_t S>
  [[nodiscard]] auto analyze(const scale_instance<S> &key,
                             const note &root) const {
    return chord.analyze(key, root);
  }


  [[nodiscard]] std::string str() const {
    if (is_rest)
      return "-(" + dur.str() + ")";
    auto a = chord.analyze();
    auto name = a.empty() ? "?" : a[0].str();
    auto s = name + "(" + dur.str() + ")";
    if (is_tied)
      s += "~";
    return s;
  }

  [[nodiscard]] std::string notes_str() const {
    if (is_rest)
      return "-(" + dur.str() + ")";
    std::string s;
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0)
        s += ' ';
      s += chord[i].str();
    }
    s += "(" + dur.str() + ")";
    if (is_tied)
      s += "~";
    return s;
  }

  friend std::ostream &operator<<(std::ostream &os, const chord_event &ce) {
    return os << ce.str();
  }
};

template <std::size_t N>
[[nodiscard]] constexpr chord_event<N>
operator*(const chord_instance<N> &c, const duration &d) noexcept {
  return {c, d, false, false};
}

template <std::size_t N>
[[nodiscard]] constexpr chord_event<N + 1>
operator*(const slash_chord_instance<N> &sc, const duration &d) noexcept {
  chord_instance<N + 1> combined{};
  auto all = sc.all_notes();
  std::copy(all.begin(), all.end(), combined.notes.begin());
  return {combined, d, false, false};
}

[[nodiscard]] inline constexpr chord_event<1>
chord_rest(duration d) noexcept {
  return {{}, d, true, false};
}


template <typename... Events> struct chord_sequence {
  std::tuple<Events...> events;
  static constexpr std::size_t length = sizeof...(Events);

  template <std::size_t I>
  [[nodiscard]] constexpr const auto &get() const noexcept {
    return std::get<I>(events);
  }

  template <typename F> constexpr void for_each(F &&f) const {
    std::apply([&](const auto &...ev) { (f(ev), ...); }, events);
  }


  template <std::size_t M>
  [[nodiscard]] constexpr auto
  operator|(const chord_event<M> &ev) const noexcept {
    return std::apply(
        [&](const auto &...es) {
          return chord_sequence<Events..., chord_event<M>>{
              std::tuple{es..., ev}};
        },
        events);
  }

  template <typename... Other>
  [[nodiscard]] constexpr auto
  operator|(const chord_sequence<Other...> &other) const noexcept {
    return std::apply(
        [&](const auto &...a) {
          return std::apply(
              [&](const auto &...b) {
                return chord_sequence<Events..., Other...>{
                    std::tuple{a..., b...}};
              },
              other.events);
        },
        events);
  }


  [[nodiscard]] constexpr duration total_duration() const noexcept {
    duration sum{0, 1};
    for_each([&](const auto &ev) { sum = sum + ev.dur; });
    return sum;
  }

  [[nodiscard]] std::string names() const {
    std::string result;
    for_each([&](const auto &ev) {
      if (!result.empty())
        result += " - ";
      if (ev.is_rest) {
        result += "-";
        return;
      }
      auto a = ev.chord.analyze();
      result += a.empty() ? "?" : a[0].str();
    });
    return result;
  }

  template <std::size_t S>
  [[nodiscard]] std::string roman(const scale_instance<S> &key) const {
    std::string result;
    for_each([&](const auto &ev) {
      if (!result.empty())
        result += " - ";
      if (ev.is_rest) {
        result += "-";
        return;
      }
      auto a = ev.chord.analyze(key);
      result += a.empty() ? "?" : a[0].str();
    });
    return result;
  }


  template <typename F>
  constexpr void walk(time_signature ts, F &&f) const;


  [[nodiscard]] std::string str() const {
    std::string result;
    for_each([&](const auto &ev) {
      if (!result.empty())
        result += ' ';
      result += ev.str();
    });
    return result;
  }

  [[nodiscard]] std::string notes_str() const {
    std::string result;
    for_each([&](const auto &ev) {
      if (!result.empty())
        result += ' ';
      result += ev.notes_str();
    });
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const chord_sequence &cs) {
    return os << cs.str();
  }
};


template <std::size_t A, std::size_t B>
[[nodiscard]] constexpr auto
operator|(const chord_event<A> &a, const chord_event<B> &b) noexcept {
  return chord_sequence<chord_event<A>, chord_event<B>>{std::tuple{a, b}};
}

}


template <std::size_t N>
struct std::formatter<musicpp::chord_event<N>>
    : std::formatter<std::string> {
  auto format(const musicpp::chord_event<N> &ce, auto &ctx) const {
    return std::formatter<std::string>::format(ce.str(), ctx);
  }
};

template <typename... Events>
struct std::formatter<musicpp::chord_sequence<Events...>>
    : std::formatter<std::string> {
  auto format(const musicpp::chord_sequence<Events...> &cs, auto &ctx) const {
    return std::formatter<std::string>::format(cs.str(), ctx);
  }
};
