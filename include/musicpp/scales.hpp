#pragma once
#include "chords.hpp"
#include "degree.hpp"
#include "intervals.hpp"
#include "notes.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <ostream>
#include <string>

namespace musicpp {

template <std::size_t N> struct scale_instance;

template <std::size_t N> struct scale_pattern {
  std::array<interval, N> intervals;

  template <std::size_t M>
  [[nodiscard]] constexpr scale_pattern mode() const noexcept {
    static_assert(M < N, "Mode index must be less than scale size");
    scale_pattern result{};
    auto offset = intervals[M];
    for (std::size_t i = 0; i < N; ++i) {
      auto iv = intervals[(i + M) % N] - offset;
      if (iv.fifths * 4 + iv.octaves * 7 < 0) {
        iv = iv + musicpp::intervals::P8;
      }
      result.intervals[i] = iv;
    }
    return result;
  }

  template <degree Deg, std::size_t Tones>
  [[nodiscard]] constexpr chord_pattern<Tones>
  chord_on() const noexcept {
    static_assert(Deg.num >= 1 && static_cast<std::size_t>(Deg.num) <= N,
                  "Scale degree out of range");
    constexpr auto d = static_cast<std::size_t>(Deg.num - 1);
    chord_pattern<Tones> result{};
    auto root = intervals[d % N];
    for (std::size_t i = 0; i < Tones; ++i) {
      std::size_t idx = (d + i * 2) % N;
      auto iv = intervals[idx] - root;
      std::size_t raw_steps = (d + i * 2);
      auto octave_bump = static_cast<std::int8_t>(raw_steps / N);
      if (iv.fifths * 4 + iv.octaves * 7 < 0) {
        ++octave_bump;
      }
      iv = iv + interval(0, octave_bump);
      result.intervals[i] = iv;
    }
    return result;
  }
};

template <std::size_t N> struct scale_instance {
  note root;
  std::array<note, N> notes;

  [[nodiscard]] constexpr const note &operator[](std::size_t index) const noexcept {
    return notes[index];
  }
  [[nodiscard]] constexpr note &operator[](std::size_t index) noexcept {
    return notes[index];
  }

  [[nodiscard]] static constexpr std::size_t size() noexcept { return N; }
  [[nodiscard]] constexpr auto begin() const noexcept { return notes.begin(); }
  [[nodiscard]] constexpr auto end() const noexcept { return notes.end(); }
  [[nodiscard]] constexpr auto begin() noexcept { return notes.begin(); }
  [[nodiscard]] constexpr auto end() noexcept { return notes.end(); }

  [[nodiscard]] constexpr bool contains(const note &target) const noexcept {
    return std::ranges::any_of(notes, [&](const note &n) {
      return n.get_pitch() == target.get_pitch() &&
             n.get_fifth() == target.get_fifth();
    });
  }

  [[nodiscard]] constexpr bool
  contains_enharmonic(const note &target) const noexcept {
    return std::ranges::any_of(notes, [&](const note &n) {
      return n.is_enharmonic(target);
    });
  }

  [[nodiscard]] constexpr degree
  degree_of(const note &target) const noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      if (notes[i].get_pitch() == target.get_pitch() &&
          notes[i].get_fifth() == target.get_fifth()) {
        return degree{static_cast<int>(i) + 1};
      }
    }
    return degree{};
  }

  template <degree Deg, std::size_t Tones>
  [[nodiscard]] constexpr chord_instance<Tones>
  chord_on() const noexcept {
    static_assert(Deg.num >= 1 && static_cast<std::size_t>(Deg.num) <= N,
                  "Scale degree out of range");
    constexpr auto d = static_cast<std::size_t>(Deg.num - 1);
    chord_instance<Tones> result{};
    for (std::size_t i = 0; i < Tones; ++i) {
      std::size_t idx = (d + i * 2) % N;
      auto n = notes[idx];
      if (i > 0) {
        while (n.get_midi_pitch() <= result.notes[i - 1].get_midi_pitch()) {
          n = n + intervals::P8;
        }
      } else {
        while (n.get_midi_pitch() < root.get_midi_pitch()) {
          n = n + intervals::P8;
        }
      }
      result.notes[i] = n;
    }
    return result;
  }

  [[nodiscard]] constexpr bool is_diatonic(const note &n) const noexcept {
    return contains(n);
  }

  template <std::size_t M>
  [[nodiscard]] constexpr bool
  is_diatonic(const chord_instance<M> &chord) const noexcept {
    return std::ranges::all_of(chord.notes, [this](const note &n) {
      return contains(n);
    });
  }

  [[nodiscard]] constexpr scale_instance
  simplify(accidental_preference pref =
               accidental_preference::natural) const noexcept {
    scale_instance result{};
    result.root = root.simplify(pref);
    for (std::size_t i = 0; i < N; ++i) {
      result.notes[i] = notes[i].simplify(pref);
    }
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const scale_instance &s) {
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0)
        os << ' ';
      os << s.notes[i];
    }
    return os;
  }
};

template <std::size_t N>
[[nodiscard]] constexpr auto operator+(const note &root,
                                       const scale_pattern<N> &pattern) noexcept {
  scale_instance<N> result{};
  result.root = root;
  for (std::size_t i = 0; i < N; ++i) {
    result.notes[i] = root + pattern.intervals[i];
  }
  return result;
}

namespace scale_patterns {
using namespace intervals;

constexpr auto major =
    scale_pattern<7>{{{P1, M2, M3, P4, P5, M6, M7}}};
constexpr auto dorian = major.mode<1>();
constexpr auto phrygian = major.mode<2>();
constexpr auto lydian = major.mode<3>();
constexpr auto mixolydian = major.mode<4>();
constexpr auto natural_minor = major.mode<5>();
constexpr auto locrian = major.mode<6>();

constexpr auto harmonic_minor =
    scale_pattern<7>{{{P1, M2, m3, P4, P5, m6, M7}}};
constexpr auto melodic_minor =
    scale_pattern<7>{{{P1, M2, m3, P4, P5, M6, M7}}};

constexpr auto phrygian_dominant = harmonic_minor.mode<4>();
constexpr auto lydian_sharp2 = harmonic_minor.mode<5>();

constexpr auto lydian_dominant = melodic_minor.mode<3>();
constexpr auto altered = melodic_minor.mode<6>();

constexpr auto major_pentatonic =
    scale_pattern<5>{{{P1, M2, M3, P5, M6}}};
constexpr auto minor_pentatonic =
    scale_pattern<5>{{{P1, m3, P4, P5, m7}}};

constexpr auto whole_tone =
    scale_pattern<6>{{{P1, M2, M3, A4, A5, m7}}};
constexpr auto blues =
    scale_pattern<6>{{{P1, m3, P4, d5, P5, m7}}};

constexpr auto chromatic = scale_pattern<12>{
    {{P1, m2, M2, m3, M3, P4, d5, P5, m6, M6, m7, M7}}};

constexpr auto bebop_dominant =
    scale_pattern<8>{{{P1, M2, M3, P4, P5, M6, m7, M7}}};
constexpr auto bebop_major =
    scale_pattern<8>{{{P1, M2, M3, P4, P5, m6, M6, M7}}};

}



}

template <std::size_t N>
struct std::formatter<musicpp::scale_instance<N>>
    : std::formatter<std::string> {
  auto format(const musicpp::scale_instance<N> &s, auto &ctx) const {
    std::string str;
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0)
        str += ' ';
      str += s[i].str();
    }
    return std::formatter<std::string>::format(str, ctx);
  }
};
