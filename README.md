# music-cpp

A header-only C++20 music theory library.

## Features

- **Intervals** — Fifth-based representation with arithmetic, enharmonic comparison, and standard notation (`P5`, `M3`, `m7`, etc.)
- **Notes** — Pitch spelling, MIDI pitch conversion, octave management, and enharmonic simplification
- **Chords** — 30+ chord patterns, inversions, voicing alterations, automatic chord name recognition, and Roman numeral analysis
- **Scales** — Major, all diatonic modes, harmonic/melodic minor, pentatonic, blues, whole tone, chromatic, bebop, and diatonic chord construction
- **Melody** — Note sequences with durations, rests, ties, and transformations (transpose, retrograde, invert, augment, diminish, repeat)
- **Chord sequences** — Heterogeneous chord event streams with duration, slash chords, analysis delegation, and iteration
- **Progressions** — Abstract degree-based progressions (`I–IV–V–I`) that can be realized into any key with a single call
- **Timing** — Time signatures (simple/compound/irregular), tempo, metric position tracking, and bar-aware `walk()` traversal
- **Duration** — Fractional note values with dotted, double-dotted, and triplet variants

## Requirements

- A C++20 compliant compiler (tested with MSVC 19.40+, should work with GCC 13+ and Clang 17+)
- [xmake](https://xmake.io/) build system
- [Boost.UT](https://github.com/boost-ext/ut) (automatically fetched by xmake, only needed for tests)

## Quick Start

### Build

```bash
xmake build
```

### Run the example

```bash
xmake run example
```

### Run tests

```bash
xmake build -y --test
xmake test
```

## Usage

music-cpp is header-only. Add the `include/` directory to your include path and you're ready to go:

```cpp
#include <musicpp/notes.hpp>
#include <musicpp/chords.hpp>
#include <musicpp/scales.hpp>
```

### Notes & Intervals

```cpp
using namespace musicpp;
using namespace musicpp::notes;
using namespace musicpp::intervals;

constexpr auto middle_c = C(4);           // C4
constexpr auto e4 = middle_c + M3;        // E4
constexpr auto g4 = middle_c + P5;        // G4

static_assert(middle_c.get_midi_pitch() == 60);
static_assert(e4.is_enharmonic(Fb(4)));   // E4 ≡ Fb4
```

### Chords

```cpp
using namespace musicpp::chord_patterns;

auto c_major = C(4) + major_triad;       // C E G
auto dm7     = D(4) + min7;              // D F A C
auto g7      = G(3) + dom7;              // G B D F

// Slash chords (inversions with specified bass)
auto f_over_a = (F(3) + major_triad) / A(2);  // F/A

// Automatic analysis
auto analysis = c_major.analyze();        // "C"
auto dm7_name = dm7.analyze();            // "Dm7"
```

### Scales & Roman Numerals

```cpp
using namespace musicpp::scale_patterns;

auto key = C(4) + major;                 // C major scale

// Build diatonic chords
auto I   = key.chord_on<1, 3>();         // C major triad
auto ii7 = key.chord_on<2, 4>();         // Dm7

// Roman numeral analysis
auto result = c_major.analyze(key);      // "I"
```

### Melody

```cpp
using namespace musicpp::durations;

// Twinkle Twinkle Little Star (first phrase)
auto melody = C(4)*q | C(4)*q | G(4)*q | G(4)*q
            | A(4)*q | A(4)*q | G(4)*h;

std::cout << melody;                     // prints all notes with durations
std::cout << melody.total_duration();    // 2/1 (2 whole notes = 8 beats)

// Transformations
auto up   = melody + M3;                 // transpose up a major 3rd
auto rev  = melody.retrograde();         // reverse the melody
auto inv  = melody.invert(C(4));         // invert around C4
auto aug  = melody.augment();            // double all durations
```

### Chord Sequences

```cpp
auto pop = (C(4) + major_triad) * h
         | (A(3) + minor_triad) * h
         | (F(3) + major_triad) * h
         | (G(3) + major_triad) * h;

std::cout << pop.names();               // "C - Am - F - G"
std::cout << pop.roman(key);            // "I - vi - IV - V"
```

### Progressions

```cpp
#include <musicpp/progressions.hpp>

using musicpp::step, musicpp::b;

// Define an abstract progression
auto jazz = step<2>(min7, w)
          | step<5>(dom7, w)
          | step<1>(maj7, w);

// Realize in any key
auto in_c = jazz.realize(C(4) + major);  // Dm7 - G7 - Cmaj7
auto in_g = jazz.realize(G(4) + major);  // Am7 - D7 - Gmaj7

// Borrowed chords with altered degrees
auto borrowed = step<1>(major_triad, h)
              | step<b(6)>(major_triad, h);  // I - bVI
```

### Timing

```cpp
#include <musicpp/timing.hpp>

using namespace musicpp::time_signatures;
using namespace musicpp::tempos;

auto t = tempo{120};
std::cout << t.ms(quarter);            // 500.0 ms
std::cout << t.bar_seconds(common);    // 2.00 s (one 4/4 bar at 120 BPM)

// Walk a melody with metric position tracking
melody.walk(common, [](const auto &ev, auto pos) {
    std::cout << std::format("bar {} beat {}: {}",
        pos.bar + 1, pos.beat_index(common) + 1, ev.str());
});
```

## Project Structure

```
music-cpp/
├── include/musicpp/      # Library headers (public API)
│   ├── intervals.hpp     # Interval type and predefined constants
│   ├── notes.hpp         # Note type and predefined pitch names
│   ├── degree.hpp        # Scale degree with b()/s() alteration helpers
│   ├── duration.hpp      # Fractional duration type
│   ├── chords.hpp        # Chord patterns, instances, analysis engine
│   ├── scales.hpp        # Scale patterns, instances, diatonic chord builder
│   ├── melody.hpp        # Melody sequences and transformations
│   ├── chord_sequence.hpp# Chord event sequences
│   ├── progressions.hpp  # Abstract degree-based progressions
│   └── timing.hpp        # Time signatures, tempo, metric position, walk()
├── example/              # Example programs
│   └── song.cpp          # Full chord transcription demo
├── test/                 # Unit tests (Boost.UT)
│   ├── intervals_test.cpp
│   ├── notes_test.cpp
│   ├── chords_test.cpp
│   ├── scales_test.cpp
│   ├── duration_test.cpp
│   ├── degree_test.cpp
│   ├── melody_test.cpp
│   ├── chord_sequence_test.cpp
│   ├── timing_test.cpp
│   └── progressions_test.cpp
└── xmake.lua             # Build configuration
```

## Design Principles

- **Constexpr everything** — Notes, intervals, chords, scales, and durations are all usable in constant expressions and as non-type template parameters
- **Zero runtime overhead** — Compile-time computation wherever possible; no heap allocation in core types
- **Expressive DSL** — Operator overloading (`+`, `*`, `|`, `/`) creates readable music notation in code
- **Orthogonal composition** — Each module is independent; combine them freely without unnecessary coupling
- **Type-safe heterogeneity** — Chord sequences and melodies use tuple-based storage to preserve per-event chord sizes at compile time

## License

This project is licensed under the [MIT License](LICENSE).
