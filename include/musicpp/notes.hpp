#pragma once
#include "intervals.hpp"
#include <array>
#include <cstdint>
#include <format>
#include <ostream>
#include <string>

namespace musicpp {

enum class accidental_preference { natural, sharp, flat };

struct note {
  std::int8_t m_fifth{0};
  std::int8_t m_octave{0};
  constexpr note() noexcept = default;
  constexpr note(std::int8_t fifth, std::int8_t octave) noexcept
      : m_fifth(fifth), m_octave(octave) {}

  [[nodiscard]] constexpr std::int8_t get_fifth() const noexcept {
    return m_fifth;
  }
  [[nodiscard]] constexpr std::int8_t get_octave() const noexcept {
    return m_octave;
  }

  [[nodiscard]] constexpr note
  operator()(std::int8_t octave) const noexcept {
    return *this + interval(0, octave);
  }


  [[nodiscard]] friend constexpr note
  operator+(const note &lhs, const interval &rhs) noexcept {
    return note(static_cast<std::int8_t>(lhs.m_fifth + rhs.fifths),
                    static_cast<std::int8_t>(lhs.m_octave + rhs.octaves));
  }

  [[nodiscard]] friend constexpr note
  operator-(const note &lhs, const interval &rhs) noexcept {
    return note(static_cast<std::int8_t>(lhs.m_fifth - rhs.fifths),
                    static_cast<std::int8_t>(lhs.m_octave - rhs.octaves));
  }

  [[nodiscard]] friend constexpr interval
  operator-(const note &lhs, const note &rhs) noexcept {
    return interval(
        static_cast<std::int8_t>(lhs.m_fifth - rhs.m_fifth),
        static_cast<std::int8_t>(lhs.m_octave - rhs.m_octave));
  }

  [[nodiscard]] constexpr std::int8_t get_midi_pitch() const noexcept {
    return static_cast<std::int8_t>(m_fifth * 7 + m_octave * 12 + 12);
  }

  [[nodiscard]] constexpr std::int8_t get_pitch() const noexcept {
    auto pitch = static_cast<std::int8_t>((m_fifth * 7) % 12);
    return static_cast<std::int8_t>((pitch + 12) % 12);
  }

  [[nodiscard]] constexpr bool
  is_enharmonic(const note &other) const noexcept {
    return get_pitch() == other.get_pitch();
  }

  [[nodiscard]] constexpr note
  simplify(accidental_preference pref = accidental_preference::natural) const noexcept {
    int offset = (pref == accidental_preference::sharp)  ? 1
               : (pref == accidental_preference::flat)   ? 6
               :                                          5;
    int shifted = m_fifth + offset;
    int adjust = (shifted >= 0) ? shifted / 12 : (shifted - 11) / 12;
    return note(
        static_cast<std::int8_t>(m_fifth - adjust * 12),
        static_cast<std::int8_t>(m_octave + adjust * 7)
    );
  }

  constexpr auto operator<=>(const note &) const noexcept = default;

  [[nodiscard]] constexpr std::string str() const {
    constexpr std::array<const char *, 7> names = {"F", "C", "G", "D",
                                                    "A", "E", "B"};
    std::int64_t shifted_fifth = m_fifth + 1;
    std::int64_t base_index = ((shifted_fifth % 7) + 7) % 7;
    std::int64_t accidentals =
        (shifted_fifth >= 0) ? shifted_fifth / 7 : (shifted_fifth - 6) / 7;
    std::string result = names[base_index];
    if (accidentals > 0) {
      for (std::int64_t i = 0; i < accidentals; ++i)
        result += "#";
    } else if (accidentals < 0) {
      for (std::int64_t i = 0; i < -accidentals; ++i)
        result += "b";
    }
    int display_octave = (get_midi_pitch() - get_pitch()) / 12 - 1;
    result += std::to_string(display_octave);
    return result;
  }

  [[nodiscard]] constexpr std::string pitch_name() const {
    constexpr std::array<const char *, 7> names = {"F", "C", "G", "D",
                                                    "A", "E", "B"};
    std::int64_t shifted_fifth = m_fifth + 1;
    std::int64_t base_index = ((shifted_fifth % 7) + 7) % 7;
    std::int64_t accidentals =
        (shifted_fifth >= 0) ? shifted_fifth / 7 : (shifted_fifth - 6) / 7;
    std::string result = names[base_index];
    if (accidentals > 0) {
      for (std::int64_t i = 0; i < accidentals; ++i)
        result += "#";
    } else if (accidentals < 0) {
      for (std::int64_t i = 0; i < -accidentals; ++i)
        result += "b";
    }
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os, const note &n) {
    return os << n.str();
  }
};
namespace notes {
constexpr auto C = note(0, 0);
constexpr auto G = note(1, 0);
constexpr auto D = note(2, -1);
constexpr auto A = note(3, -1);
constexpr auto E = note(4, -2);
constexpr auto B = note(5, -2);
constexpr auto Fs = note(6, -3);
constexpr auto Cs = note(7, -4);
constexpr auto Gs = note(8, -4);
constexpr auto Ds = note(9, -5);
constexpr auto As = note(10, -5);

constexpr auto F = note(-1, 1);
constexpr auto Bb = note(-2, 2);
constexpr auto Eb = note(-3, 2);
constexpr auto Ab = note(-4, 3);
constexpr auto Db = note(-5, 3);
constexpr auto Gb = note(-6, 4);
}

}

template <>
struct std::formatter<musicpp::note> : std::formatter<std::string> {
  auto format(const musicpp::note &n, auto &ctx) const {
    return std::formatter<std::string>::format(n.str(), ctx);
  }
};