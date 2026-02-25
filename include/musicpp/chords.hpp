#pragma once
#include "degree.hpp"
#include "intervals.hpp"
#include "notes.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <format>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace musicpp {
template <std::size_t N> struct slash_chord_instance;
template <std::size_t S> struct scale_instance;

template <std::size_t N> struct chord_pattern {
  std::array<interval, N> intervals;


template <std::size_t M>
[[nodiscard]] constexpr chord_pattern<N> inversion() const noexcept {
    static_assert(M < N, "Inversion number must be less than the number of chord tones");
    
    auto result = *this;
    auto& ivs = result.intervals;

    std::rotate(ivs.begin(), ivs.begin() + M, ivs.end());
    for (std::size_t i = N - M; i < N; ++i) {
        ivs[i] += musicpp::intervals::P8;
    }
    return result;
}

  template <std::size_t I>
  [[nodiscard]] constexpr chord_pattern<N>
  alter(const interval &new_interval) const noexcept {
    static_assert(I < N, "Alter index must be within chord tone count");
    auto result = *this;
    result.intervals[I] = new_interval;
    return result;
  }

  template <std::size_t I>
  [[nodiscard]] constexpr chord_pattern<N - 1> omit() const noexcept {
    static_assert(N > 1, "Cannot omit from a single-note chord");
    static_assert(I < N, "Omit index must be within chord tone count");
    chord_pattern<N - 1> result{};
    auto it = std::copy_n(intervals.begin(), I, result.intervals.begin());
    std::copy(intervals.begin() + I + 1, intervals.end(), it);
    return result;
  }

  [[nodiscard]] constexpr chord_pattern<N + 1>
  add(const interval &iv) const noexcept {
    chord_pattern<N + 1> result{};
    std::copy(intervals.begin(), intervals.end(), result.intervals.begin());
    result.intervals[N] = iv;
    return result;
  }
};

struct chord_analysis {
  note root;
  std::string quality;
  std::optional<note> bass;
  std::int8_t inversion{0};
  std::vector<std::string> omissions;

  [[nodiscard]] std::string str() const {
    auto result = root.simplify().pitch_name() + quality;
    if (!omissions.empty()) {
      result += "(";
      for (std::size_t i = 0; i < omissions.size(); ++i) {
        if (i > 0)
          result += ",";
        result += omissions[i];
      }
      result += ")";
    }
    if (bass) {
      result += "/";
      result += bass->simplify().pitch_name();
    }
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os, const chord_analysis &a) {
    return os << a.str();
  }
};

struct analysis_result {
  std::vector<chord_analysis> interpretations;

  [[nodiscard]] bool empty() const noexcept { return interpretations.empty(); }
  [[nodiscard]] std::size_t size() const noexcept {
    return interpretations.size();
  }
  [[nodiscard]] const chord_analysis &operator[](std::size_t i) const {
    return interpretations[i];
  }
  [[nodiscard]] auto begin() const noexcept { return interpretations.begin(); }
  [[nodiscard]] auto end() const noexcept { return interpretations.end(); }

  [[nodiscard]] std::string str() const {
    if (interpretations.empty())
      return "?";
    std::string result;
    for (std::size_t i = 0; i < interpretations.size(); ++i) {
      if (i > 0)
        result += " | ";
      result += interpretations[i].str();
    }
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os, const analysis_result &r) {
    return os << r.str();
  }
};

template <std::size_t N> struct chord_instance {
  std::array<note, N> notes;

  [[nodiscard]] constexpr const note &
  operator[](std::size_t index) const noexcept {
    return notes[index];
  }
  [[nodiscard]] constexpr note &operator[](std::size_t index) noexcept {
    return notes[index];
  }

  [[nodiscard]] static constexpr std::size_t size() noexcept { return N; }
  [[nodiscard]] constexpr auto data() const noexcept { return notes.data(); }
  [[nodiscard]] constexpr auto data() noexcept { return notes.data(); }
  [[nodiscard]] constexpr auto begin() const noexcept { return notes.begin(); }
  [[nodiscard]] constexpr auto end() const noexcept { return notes.end(); }
  [[nodiscard]] constexpr auto begin() noexcept { return notes.begin(); }
  [[nodiscard]] constexpr auto end() noexcept { return notes.end(); }

  [[nodiscard]] constexpr note lowest() const noexcept {
    return *std::ranges::min_element(notes, {}, &note::get_midi_pitch);
  }

  [[nodiscard]] constexpr note highest() const noexcept {
    return *std::ranges::max_element(notes, {}, &note::get_midi_pitch);
  }

  [[nodiscard]] constexpr bool contains(const note &target) const noexcept {
    return std::ranges::any_of(notes,
                               [&](const note &n) { return n == target; });
  }

  [[nodiscard]] constexpr bool
  contains_enharmonic(const note &target) const noexcept {
    return std::ranges::any_of(
        notes, [&](const note &n) { return n.is_enharmonic(target); });
  }

  [[nodiscard]] constexpr chord_instance
  simplify(accidental_preference pref =
               accidental_preference::natural) const noexcept {
    chord_instance result{};

    std::ranges::transform(notes, result.notes.begin(), [pref](const auto& n) {
        return n.simplify(pref);
    });
    return result;
  }

  [[nodiscard]] analysis_result analyze() const;
  [[nodiscard]] std::optional<chord_analysis> analyze(const note &root) const;
  template <std::size_t S>
  [[nodiscard]] auto analyze(const scale_instance<S> &key) const;
  template <std::size_t S>
  [[nodiscard]] auto analyze(const scale_instance<S> &key, const note &root) const;
  [[nodiscard]] constexpr auto operator/(const note &bass) const;

  friend std::ostream &operator<<(std::ostream &os, const chord_instance &c) {
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0)
        os << ' ';
      os << c.notes[i];
    }
    return os;
  }
};

template <std::size_t N> struct slash_chord_instance {
  chord_instance<N> chord;
  note bass;

  constexpr slash_chord_instance(const chord_instance<N> &ch,
                                 const note &bass_note)
      : chord(ch), bass(bass_note) {
    if (chord.contains_enharmonic(bass)) {
      for (auto &n : chord) {
        while (n.get_midi_pitch() <= bass.get_midi_pitch()) {
          n = n + intervals::P8;
        }
      }
    }
    if (bass.get_midi_pitch() >= chord.lowest().get_midi_pitch()) {
      throw "slash chord bass must be lower than all chord tones";
    }
  }

  [[nodiscard]] static constexpr std::size_t size() noexcept { return N; }
  [[nodiscard]] constexpr auto begin() const noexcept { return chord.begin(); }
  [[nodiscard]] constexpr auto end() const noexcept { return chord.end(); }

  [[nodiscard]] constexpr const note &
  operator[](std::size_t index) const noexcept {
    return chord[index];
  }

  [[nodiscard]] constexpr std::array<note, N + 1> all_notes() const noexcept {
    std::array<note, N + 1> result;
    result[0] = bass;
    std::copy(chord.begin(), chord.end(), result.begin() + 1);
    return result;
  }

  [[nodiscard]] constexpr slash_chord_instance
  simplify(accidental_preference pref =
               accidental_preference::natural) const noexcept {
    slash_chord_instance result = *this;
    result.chord = chord.simplify(pref);
    result.bass = bass.simplify(pref);
    return result;
  }

  [[nodiscard]] analysis_result analyze() const;
  [[nodiscard]] std::optional<chord_analysis> analyze(const note &root) const;
  template <std::size_t S>
  [[nodiscard]] auto analyze(const scale_instance<S> &key) const;
  template <std::size_t S>
  [[nodiscard]] auto analyze(const scale_instance<S> &key, const note &root) const;

  friend std::ostream &operator<<(std::ostream &os,
                                  const slash_chord_instance &sc) {
    os << sc.chord << '/' << sc.bass;
    return os;
  }
};

template <std::size_t N>
[[nodiscard]] constexpr auto
chord_instance<N>::operator/(const note &bass) const {
  return slash_chord_instance<N>{*this, bass};
}

template <std::size_t N>
[[nodiscard]] constexpr auto
operator+(const note &note, const chord_pattern<N> &pattern) noexcept {
  chord_instance<N> result{};
  for (std::size_t i = 0; i < N; ++i) {
    result.notes[i] = note + pattern.intervals[i];
  }
  return result;
}

namespace detail {
struct chord_info {
  std::string_view name;
  std::uint16_t pitch_class_set;
  std::uint8_t tone_count;
  std::array<std::int8_t, 7> interval_semitones;
};

template <std::size_t N>
consteval chord_info make_chord_info(std::string_view name,
                                     const chord_pattern<N> &pattern) {
  std::uint16_t pcs = 0;
  std::array<std::int8_t, 7> semis{};
  semis.fill(-1);
  for (std::size_t i = 0; i < N; ++i) {
    int s = ((pattern.intervals[i].fifths * 7) % 12 + 12) % 12;
    pcs |= static_cast<std::uint16_t>(1u << s);
    if (i < 7)
      semis[i] = static_cast<std::int8_t>(s);
  }
  return {name, pcs, static_cast<std::uint8_t>(N), semis};
}

inline std::string semitone_to_omission_name(std::int8_t semi) {
  switch (semi) {
  case 1: case 2: return "no9";
  case 3: case 4: return "no3";
  case 5:         return "no11";
  case 6: case 7: return "no5";
  case 8: case 9: return "no13";
  case 10: case 11: return "no7";
  default:        return "no?";
  }
}

inline std::int8_t detect_inversion(std::int8_t bass_semi, const chord_info &info) {
  for (std::int8_t i = 1; i < info.tone_count && i < 7; ++i) {
    if (info.interval_semitones[i] == bass_semi)
      return i;
  }
  return 0;
}

template <std::size_t M>
[[nodiscard]] inline analysis_result
analyze_all(const std::array<note, M> &notes);

template <std::size_t M>
[[nodiscard]] inline std::optional<chord_analysis>
analyze_with_root(const std::array<note, M> &notes, const note &root);
}

template <std::size_t N>
[[nodiscard]] inline analysis_result chord_instance<N>::analyze() const {
  return detail::analyze_all(notes);
}

template <std::size_t N>
[[nodiscard]] inline std::optional<chord_analysis>
chord_instance<N>::analyze(const note &root) const {
  return detail::analyze_with_root(notes, root);
}

template <std::size_t N>
[[nodiscard]] inline analysis_result slash_chord_instance<N>::analyze() const {
  auto result = detail::analyze_all(all_notes());
  if (!result.empty())
    return result;
  auto chord_result = chord.analyze();
  for (auto &a : chord_result.interpretations) {
    a.quality += "/" + bass.simplify().pitch_name();
  }
  return chord_result;
}

template <std::size_t N>
[[nodiscard]] inline std::optional<chord_analysis>
slash_chord_instance<N>::analyze(const note &root) const {
  auto result = detail::analyze_with_root(all_notes(), root);
  if (result)
    return result;
  auto chord_result = chord.analyze(root);
  if (chord_result) {
    chord_result->quality += "/" + bass.simplify().pitch_name();
  }
  return chord_result;
}

namespace chord_patterns {
constexpr auto major_triad =
    chord_pattern<3>{{intervals::P1, intervals::M3, intervals::P5}};
constexpr auto minor_triad =
    chord_pattern<3>{{intervals::P1, intervals::m3, intervals::P5}};
constexpr auto diminished_triad =
    chord_pattern<3>{{intervals::P1, intervals::m3, intervals::d5}};
constexpr auto augmented_triad =
    chord_pattern<3>{{intervals::P1, intervals::M3, intervals::A5}};
constexpr auto sus2 =
    chord_pattern<3>{{intervals::P1, intervals::M2, intervals::P5}};
constexpr auto sus4 =
    chord_pattern<3>{{intervals::P1, intervals::P4, intervals::P5}};

constexpr auto maj6 = major_triad.add(intervals::M6);
constexpr auto min6 = minor_triad.add(intervals::M6);

constexpr auto dom7 = major_triad.add(intervals::m7);
constexpr auto maj7 = major_triad.add(intervals::M7);
constexpr auto min7 = minor_triad.add(intervals::m7);
constexpr auto min_maj7 = minor_triad.add(intervals::M7);
constexpr auto dim7 = diminished_triad.add(intervals::d7);
constexpr auto half_dim7 = diminished_triad.add(intervals::m7);
constexpr auto aug7 = augmented_triad.add(intervals::m7);
constexpr auto aug_maj7 = augmented_triad.add(intervals::M7);
constexpr auto dom7sus4 = sus4.add(intervals::m7);

constexpr auto dom7_sharp5 = dom7.alter<2>(intervals::A5);
constexpr auto dom7_flat5 = dom7.alter<2>(intervals::d5);
constexpr auto dom7_sharp9 = dom7.add(intervals::A9);
constexpr auto dom7_flat9 = dom7.add(intervals::m9);
constexpr auto dom7_sharp5_sharp9 = dom7_sharp5.add(intervals::A9);
constexpr auto dom7_sharp5_flat9 = dom7_sharp5.add(intervals::m9);
constexpr auto dom7_flat5_flat9 = dom7_flat5.add(intervals::m9);

constexpr auto maj6_9 = maj6.add(intervals::M9);
constexpr auto min6_9 = min6.add(intervals::M9);
constexpr auto dom9 = dom7.add(intervals::M9);
constexpr auto maj9 = maj7.add(intervals::M9);
constexpr auto min9 = min7.add(intervals::M9);
constexpr auto min_maj9 = min_maj7.add(intervals::M9);
constexpr auto aug9 = aug7.add(intervals::M9);
constexpr auto dom9sus4 = dom7sus4.add(intervals::M9);

constexpr auto dom9_sharp11 = dom9.add(intervals::A11);
constexpr auto dom9_flat13 = dom9.add(intervals::m13);
constexpr auto dom9_sharp5 = dom9.alter<2>(intervals::A5);
constexpr auto dom9_flat5 = dom9.alter<2>(intervals::d5);

constexpr auto dom11 = dom9.add(intervals::P11);
constexpr auto maj11 = maj9.add(intervals::P11);
constexpr auto min11 = min9.add(intervals::P11);

constexpr auto dom13 = dom11.add(intervals::M13);
constexpr auto maj13 = maj11.add(intervals::M13);
constexpr auto min13 = min11.add(intervals::M13);
constexpr auto dom13_sharp11 = dom13.alter<4>(intervals::A11);

constexpr auto power_chord = major_triad.omit<1>();
constexpr auto dom9_no5 = dom9.omit<2>();
constexpr auto dom11_no3 = dom11.omit<1>();
constexpr auto dom13_no5 = dom13.omit<2>();
constexpr auto dom13_no5_no11 = dom13_no5.omit<3>();

constexpr auto add9 = major_triad.add(intervals::M9);
constexpr auto min_add9 = minor_triad.add(intervals::M9);
constexpr auto add11 = major_triad.add(intervals::P11);
constexpr auto add13 = major_triad.add(intervals::M13);
}

namespace detail {
inline constexpr auto chord_db = std::to_array<chord_info>({
    make_chord_info("13", chord_patterns::dom13),
    make_chord_info("maj13", chord_patterns::maj13),
    make_chord_info("m13", chord_patterns::min13),
    make_chord_info("13#11", chord_patterns::dom13_sharp11),
    make_chord_info("11", chord_patterns::dom11),
    make_chord_info("maj11", chord_patterns::maj11),
    make_chord_info("m11", chord_patterns::min11),
    make_chord_info("9#11", chord_patterns::dom9_sharp11),
    make_chord_info("9b13", chord_patterns::dom9_flat13),
    make_chord_info("9", chord_patterns::dom9),
    make_chord_info("maj9", chord_patterns::maj9),
    make_chord_info("m9", chord_patterns::min9),
    make_chord_info("m(maj9)", chord_patterns::min_maj9),
    make_chord_info("aug9", chord_patterns::aug9),
    make_chord_info("9sus4", chord_patterns::dom9sus4),
    make_chord_info("9b5", chord_patterns::dom9_flat5),
    make_chord_info("6/9", chord_patterns::maj6_9),
    make_chord_info("m6/9", chord_patterns::min6_9),
    make_chord_info("7b9", chord_patterns::dom7_flat9),
    make_chord_info("7#9", chord_patterns::dom7_sharp9),
    make_chord_info("7#5b9", chord_patterns::dom7_sharp5_flat9),
    make_chord_info("7#5#9", chord_patterns::dom7_sharp5_sharp9),
    make_chord_info("7b5b9", chord_patterns::dom7_flat5_flat9),
    make_chord_info("add9", chord_patterns::add9),
    make_chord_info("m(add9)", chord_patterns::min_add9),
    make_chord_info("add11", chord_patterns::add11),
    make_chord_info("6", chord_patterns::maj6),
    make_chord_info("m6", chord_patterns::min6),
    make_chord_info("7", chord_patterns::dom7),
    make_chord_info("maj7", chord_patterns::maj7),
    make_chord_info("m7", chord_patterns::min7),
    make_chord_info("m(maj7)", chord_patterns::min_maj7),
    make_chord_info("dim7", chord_patterns::dim7),
    make_chord_info("m7b5", chord_patterns::half_dim7),
    make_chord_info("aug7", chord_patterns::aug7),
    make_chord_info("maj7#5", chord_patterns::aug_maj7),
    make_chord_info("7sus4", chord_patterns::dom7sus4),
    make_chord_info("7b5", chord_patterns::dom7_flat5),
    make_chord_info("", chord_patterns::major_triad),
    make_chord_info("m", chord_patterns::minor_triad),
    make_chord_info("dim", chord_patterns::diminished_triad),
    make_chord_info("aug", chord_patterns::augmented_triad),
    make_chord_info("sus2", chord_patterns::sus2),
    make_chord_info("sus4", chord_patterns::sus4),
    make_chord_info("5", chord_patterns::power_chord),
});

struct match_result {
  const chord_info *info;
  std::vector<std::string> omissions;
};

inline std::vector<match_result>
find_matches(std::uint16_t input_pcs, std::size_t input_count) {
  std::vector<match_result> results;

  for (const auto &info : chord_db) {
    if (input_pcs == info.pitch_class_set) {
      results.push_back({&info, {}});
    }
  }
  if (!results.empty())
    return results;

  if (!(input_pcs & 1u))
    return results;

  for (const auto &info : chord_db) {
    if (info.tone_count <= input_count)
      continue;
    if ((input_pcs & info.pitch_class_set) != input_pcs)
      continue;

    auto omitted_bits = static_cast<std::uint16_t>(info.pitch_class_set & ~input_pcs);
    int omitted_count = std::popcount(omitted_bits);
    if (omitted_count > 2)
      continue;

    if (omitted_bits & 1u)
      continue;

    std::vector<std::string> omissions;
    for (std::int8_t j = 0; j < info.tone_count && j < 7; ++j) {
      auto semi = info.interval_semitones[j];
      if (semi >= 0 && (omitted_bits & (1u << semi))) {
        omissions.push_back(semitone_to_omission_name(semi));
      }
    }

    if (static_cast<int>(omissions.size()) == omitted_count) {
      results.push_back({&info, std::move(omissions)});
    }
  }
  return results;
}

inline chord_analysis build_analysis(const note &root_note, const match_result &match,
                                     const note &lowest) {
  auto root_pitch = root_note.get_pitch();
  std::optional<note> bass_note;
  std::int8_t inv = 0;

  if (root_pitch != lowest.get_pitch()) {
    bass_note = lowest;
    auto bass_semi = static_cast<std::int8_t>((lowest.get_pitch() - root_pitch + 12) % 12);
    inv = detect_inversion(bass_semi, *match.info);
  }

  return {root_note.simplify(), std::string(match.info->name),
          bass_note, inv, match.omissions};
}

template <std::size_t M>
[[nodiscard]] inline analysis_result
analyze_all(const std::array<note, M> &notes) {
  auto build_pcs = [&](std::int8_t root_pitch) -> std::uint16_t {
    std::uint16_t set = 0;
    for (std::size_t i = 0; i < M; ++i) {
      int rel = (notes[i].get_pitch() - root_pitch + 12) % 12;
      set |= static_cast<std::uint16_t>(1u << rel);
    }
    return set;
  };

  std::array<std::size_t, M> order;
  std::iota(order.begin(), order.end(), 0);
  std::ranges::sort(order, [&](std::size_t a, std::size_t b) {
    return notes[a].get_midi_pitch() < notes[b].get_midi_pitch();
  });

  analysis_result result;
  auto lowest = notes[order[0]];
  std::uint16_t tried = 0;

  auto input_count = static_cast<std::size_t>(
      std::popcount(build_pcs(notes[order[0]].get_pitch())));

  for (std::size_t idx : order) {
    auto pc = notes[idx].get_pitch();
    if (tried & (1u << pc))
      continue;
    tried |= static_cast<std::uint16_t>(1u << pc);

    auto pcs_val = build_pcs(pc);
    auto matches = find_matches(pcs_val, input_count);
    for (const auto &m : matches) {
      result.interpretations.push_back(
          build_analysis(notes[idx], m, lowest));
    }
  }

  std::ranges::stable_sort(result.interpretations,
    [](const chord_analysis &a, const chord_analysis &b) {
      if (a.omissions.size() != b.omissions.size())
        return a.omissions.size() < b.omissions.size();
      if (a.inversion != b.inversion)
        return a.inversion < b.inversion;
      return a.quality.size() < b.quality.size();
    });

  return result;
}

template <std::size_t M>
[[nodiscard]] inline std::optional<chord_analysis>
analyze_with_root(const std::array<note, M> &notes, const note &root) {
  std::uint16_t set = 0;
  auto root_pitch = root.get_pitch();
  for (std::size_t i = 0; i < M; ++i) {
    int rel = (notes[i].get_pitch() - root_pitch + 12) % 12;
    set |= static_cast<std::uint16_t>(1u << rel);
  }
  auto lowest = *std::ranges::min_element(notes, {}, &note::get_midi_pitch);
  auto input_count = static_cast<std::size_t>(std::popcount(set));
  auto matches = find_matches(set, input_count);
  if (!matches.empty()) {
    return build_analysis(root, matches[0], lowest);
  }
  return std::nullopt;
}
}


struct degree_analysis {
  chord_analysis chord;
  degree deg;
  std::string roman_numeral;

  [[nodiscard]] std::string str() const { return roman_numeral; }

  friend std::ostream &operator<<(std::ostream &os, const degree_analysis &d) {
    return os << d.str();
  }
};

struct key_analysis_result {
  std::vector<degree_analysis> interpretations;

  [[nodiscard]] bool empty() const noexcept { return interpretations.empty(); }
  [[nodiscard]] std::size_t size() const noexcept {
    return interpretations.size();
  }
  [[nodiscard]] const degree_analysis &operator[](std::size_t i) const {
    return interpretations[i];
  }
  [[nodiscard]] auto begin() const noexcept { return interpretations.begin(); }
  [[nodiscard]] auto end() const noexcept { return interpretations.end(); }

  [[nodiscard]] std::string str() const {
    if (interpretations.empty())
      return "?";
    std::string result;
    for (std::size_t i = 0; i < interpretations.size(); ++i) {
      if (i > 0)
        result += " | ";
      result += interpretations[i].str();
    }
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const key_analysis_result &r) {
    return os << r.str();
  }
};

namespace detail {

constexpr std::array<const char *, 7> roman_upper = {
    "I", "II", "III", "IV", "V", "VI", "VII"};
constexpr std::array<const char *, 7> roman_lower = {
    "i", "ii", "iii", "iv", "v", "vi", "vii"};

inline bool is_major_quality(const std::string &quality) {
  if (quality.empty()) return true;
  if (quality.starts_with("maj")) return true;
  if (quality[0] == 'm') return false;
  if (quality.starts_with("dim")) return false;
  return true;
}

inline std::string roman_quality_suffix(const std::string &quality) {
  if (quality.empty()) return "";
  if (quality == "m") return "";
  if (quality == "dim") return "\u00b0";
  if (quality == "aug") return "+";
  if (quality == "m7b5") return "\u00f87";
  if (quality == "dim7") return "\u00b07";

  if (quality.starts_with("maj")) return quality;

  if (quality.starts_with("m(")) return quality.substr(1);
  if (quality[0] == 'm') return quality.substr(1);

  if (quality.starts_with("aug")) return "+" + quality.substr(3);

  return quality;
}

template <std::size_t S>
inline degree_analysis make_degree_analysis(
    const chord_analysis &ca,
    const std::array<note, S> &scale_notes) {

  auto root_pitch = ca.root.simplify().get_pitch();

  int deg_idx = -1;
  int chromatic_offset = 0;

  for (std::size_t i = 0; i < S; ++i) {
    auto sp = scale_notes[i].get_pitch();
    if (sp == root_pitch) {
      deg_idx = static_cast<int>(i);
      break;
    }
  }

  if (deg_idx == -1) {
    for (std::size_t i = 0; i < S; ++i) {
      auto sp = scale_notes[i].get_pitch();
      int diff = (root_pitch - sp + 12) % 12;
      if (diff == 1) {
        deg_idx = static_cast<int>(i);
        chromatic_offset = 1;
        break;
      }
      if (diff == 11) {
        deg_idx = static_cast<int>(i);
        chromatic_offset = -1;
        break;
      }
    }
  }

  degree deg;
  std::string roman;

  if (deg_idx >= 0 && deg_idx < static_cast<int>(S)) {
    deg = degree{deg_idx + 1, chromatic_offset};

    roman += deg.prefix();

    std::size_t roman_idx = static_cast<std::size_t>(deg_idx % 7);
    if (roman_idx < 7) {
      if (is_major_quality(ca.quality))
        roman += roman_upper[roman_idx];
      else
        roman += roman_lower[roman_idx];
    } else {
      roman += "?";
    }

    roman += roman_quality_suffix(ca.quality);

    if (!ca.omissions.empty()) {
      roman += "(";
      for (std::size_t i = 0; i < ca.omissions.size(); ++i) {
        if (i > 0) roman += ",";
        roman += ca.omissions[i];
      }
      roman += ")";
    }
  } else {
    roman = ca.str();
  }

  return {ca, deg, roman};
}

template <std::size_t M, std::size_t S>
[[nodiscard]] inline key_analysis_result
analyze_in_key(const std::array<note, M> &notes,
               const std::array<note, S> &scale_notes) {
  auto ar = analyze_all(notes);
  key_analysis_result result;
  for (const auto &ca : ar.interpretations) {
    result.interpretations.push_back(make_degree_analysis(ca, scale_notes));
  }
  return result;
}

template <std::size_t M, std::size_t S>
[[nodiscard]] inline std::optional<degree_analysis>
analyze_in_key_with_root(const std::array<note, M> &notes,
                         const note &root,
                         const std::array<note, S> &scale_notes) {
  auto ca = analyze_with_root(notes, root);
  if (!ca)
    return std::nullopt;
  return make_degree_analysis(*ca, scale_notes);
}

}

template <std::size_t N>
template <std::size_t S>
[[nodiscard]] inline auto
chord_instance<N>::analyze(const scale_instance<S> &key) const {
  return detail::analyze_in_key(notes, key.notes);
}

template <std::size_t N>
template <std::size_t S>
[[nodiscard]] inline auto
chord_instance<N>::analyze(const scale_instance<S> &key,
                           const note &root) const {
  return detail::analyze_in_key_with_root(notes, root, key.notes);
}


template <std::size_t N>
template <std::size_t S>
[[nodiscard]] inline auto
slash_chord_instance<N>::analyze(const scale_instance<S> &key) const {
  auto result = detail::analyze_in_key(all_notes(), key.notes);
  if (!result.empty())
    return result;
  auto chord_result = chord.analyze(key);
  for (auto &da : chord_result.interpretations) {
    da.chord.quality += "/" + bass.simplify().pitch_name();
    da = detail::make_degree_analysis(da.chord, key.notes);
  }
  return chord_result;
}

template <std::size_t N>
template <std::size_t S>
[[nodiscard]] inline auto
slash_chord_instance<N>::analyze(const scale_instance<S> &key,
                                 const note &root) const {
  auto result = detail::analyze_in_key_with_root(all_notes(), root, key.notes);
  if (result)
    return result;
  auto chord_result = chord.analyze(key, root);
  if (chord_result) {
    chord_result->chord.quality += "/" + bass.simplify().pitch_name();
    *chord_result = detail::make_degree_analysis(chord_result->chord, key.notes);
  }
  return chord_result;
}

}

template <>
struct std::formatter<musicpp::chord_analysis> : std::formatter<std::string> {
  auto format(const musicpp::chord_analysis &a, auto &ctx) const {
    return std::formatter<std::string>::format(a.str(), ctx);
  }
};

template <>
struct std::formatter<musicpp::analysis_result> : std::formatter<std::string> {
  auto format(const musicpp::analysis_result &r, auto &ctx) const {
    return std::formatter<std::string>::format(r.str(), ctx);
  }
};

template <std::size_t N>
struct std::formatter<musicpp::chord_instance<N>>
    : std::formatter<std::string> {
  auto format(const musicpp::chord_instance<N> &c, auto &ctx) const {
    std::string s;
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0)
        s += ' ';
      s += c[i].str();
    }
    return std::formatter<std::string>::format(s, ctx);
  }
};

template <std::size_t N>
struct std::formatter<musicpp::slash_chord_instance<N>>
    : std::formatter<std::string> {
  auto format(const musicpp::slash_chord_instance<N> &sc, auto &ctx) const {
    std::string s;
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0)
        s += ' ';
      s += sc[i].str();
    }
    s += '/';
    s += sc.bass.str();
    return std::formatter<std::string>::format(s, ctx);
  }
};

template <>
struct std::formatter<musicpp::degree_analysis>
    : std::formatter<std::string> {
  auto format(const musicpp::degree_analysis &d, auto &ctx) const {
    return std::formatter<std::string>::format(d.str(), ctx);
  }
};

template <>
struct std::formatter<musicpp::key_analysis_result>
    : std::formatter<std::string> {
  auto format(const musicpp::key_analysis_result &r, auto &ctx) const {
    return std::formatter<std::string>::format(r.str(), ctx);
  }
};