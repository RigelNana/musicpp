#include <boost/ut.hpp>
#include <musicpp/chords.hpp>
#include <musicpp/scales.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::notes;
    using namespace musicpp::intervals;
    using namespace musicpp::chord_patterns;
    using namespace musicpp::scale_patterns;
    using namespace std::literals;

    "major triad construction"_test = [] {
        auto chord = C(4) + major_triad;
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == E.get_pitch());
        expect(chord[2].get_pitch() == G.get_pitch());

        expect(chord[0].get_midi_pitch() == 60_i);
        expect(chord[1].get_midi_pitch() == 64_i);
        expect(chord[2].get_midi_pitch() == 67_i);
    };

    "minor triad construction"_test = [] {
        auto chord = A(4) + minor_triad;
        expect(chord[0].get_pitch() == A.get_pitch());
        expect(chord[1].get_pitch() == C.get_pitch());
        expect(chord[2].get_pitch() == E.get_pitch());
    };

    "diminished triad construction"_test = [] {
        auto chord = B(4) + diminished_triad;
        expect(chord[0].get_pitch() == B.get_pitch());
        expect(chord[1].get_pitch() == D.get_pitch());
        expect(chord[2].get_pitch() == F.get_pitch());
    };

    "augmented triad construction"_test = [] {
        auto chord = C(4) + augmented_triad;
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == E.get_pitch());
        expect(chord[2].get_pitch() == Gs.get_pitch());
    };

    "major triad first inversion"_test = [] {
        auto chord = C(4) + major_triad.inversion<1>();
        expect(chord[0].get_pitch() == E.get_pitch());
        expect(chord[1].get_pitch() == G.get_pitch());
        expect(chord[2].get_pitch() == C.get_pitch());
        expect(chord[2].get_midi_pitch() > chord[1].get_midi_pitch());
        expect(chord[1].get_midi_pitch() > chord[0].get_midi_pitch());
    };

    "major triad second inversion"_test = [] {
        auto chord = C(4) + major_triad.inversion<2>();
        expect(chord[0].get_pitch() == G.get_pitch());
        expect(chord[1].get_pitch() == C.get_pitch());
        expect(chord[2].get_pitch() == E.get_pitch());
        expect(chord[2].get_midi_pitch() > chord[1].get_midi_pitch());
        expect(chord[1].get_midi_pitch() > chord[0].get_midi_pitch());
    };

    "chord in different keys"_test = [] {
        auto g_major = G(4) + major_triad;
        expect(g_major[0].get_pitch() == G.get_pitch());
        expect(g_major[1].get_pitch() == B.get_pitch());
        expect(g_major[2].get_pitch() == D.get_pitch());

        auto d_major = D(4) + major_triad;
        expect(d_major[0].get_pitch() == D.get_pitch());
        expect(d_major[1].get_pitch() == Fs.get_pitch());
        expect(d_major[2].get_pitch() == A.get_pitch());

        auto f_major = F(4) + major_triad;
        expect(f_major[0].get_pitch() == F.get_pitch());
        expect(f_major[1].get_pitch() == A.get_pitch());
        expect(f_major[2].get_pitch() == C.get_pitch());
    };

    "chord constexpr evaluation"_test = [] {
        constexpr auto chord = note(0, 4) + major_triad;
        static_assert(chord.size() == 3);
        expect(chord.size() == 3_ul);
    };


    "sus2 chord construction"_test = [] {
        auto chord = C(4) + sus2;
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == D.get_pitch());
        expect(chord[2].get_pitch() == G.get_pitch());
    };

    "sus4 chord construction"_test = [] {
        auto chord = C(4) + sus4;
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == F.get_pitch());
        expect(chord[2].get_pitch() == G.get_pitch());
    };

    "dom7 chord construction"_test = [] {
        auto chord = C(4) + dom7;
        expect(chord.size() == 4_ul);
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == E.get_pitch());
        expect(chord[2].get_pitch() == G.get_pitch());
        expect(chord[3].get_pitch() == Bb.get_pitch());
    };

    "maj7 chord construction"_test = [] {
        auto chord = C(4) + maj7;
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == E.get_pitch());
        expect(chord[2].get_pitch() == G.get_pitch());
        expect(chord[3].get_pitch() == B.get_pitch());
    };

    "min7 chord construction"_test = [] {
        auto chord = A(4) + min7;
        expect(chord[0].get_pitch() == A.get_pitch());
        expect(chord[1].get_pitch() == C.get_pitch());
        expect(chord[2].get_pitch() == E.get_pitch());
        expect(chord[3].get_pitch() == G.get_pitch());
    };

    "dim7 chord construction"_test = [] {
        auto chord = B(4) + dim7;
        expect(chord[0].get_pitch() == B.get_pitch());
        expect(chord[1].get_pitch() == D.get_pitch());
        expect(chord[2].get_pitch() == F.get_pitch());
    };

    "dom9 chord construction"_test = [] {
        auto chord = C(4) + dom9;
        expect(chord.size() == 5_ul);
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[4].get_pitch() == D.get_pitch());
    };


    "chord lowest and highest"_test = [] {
        auto chord = C(4) + major_triad;
        expect(chord.lowest().get_pitch() == C.get_pitch());
        expect(chord.highest().get_pitch() == G.get_pitch());
    };

    "chord contains"_test = [] {
        auto chord = C(4) + major_triad;
        expect(chord.contains(C(4)));
        expect(chord.contains(E(4)));
        expect(chord.contains(G(4)));
        expect(!chord.contains(D(4)));
    };

    "chord contains_enharmonic"_test = [] {
        auto chord = C(4) + major_triad;
        expect(chord.contains_enharmonic(C(4)));
        expect(!chord.contains_enharmonic(Db(4)));
    };


    "chord pattern alter"_test = [] {
        auto chord = C(4) + major_triad.alter<2>(A5);
        expect(chord[2].get_pitch() == Gs.get_pitch());
    };

    "chord pattern omit"_test = [] {
        auto pattern = major_triad.omit<1>();
        expect(pattern.intervals.size() == 2_ul);
        auto chord = C(4) + pattern;
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == G.get_pitch());
    };

    "chord pattern add"_test = [] {
        auto pattern = major_triad.add(M9);
        expect(pattern.intervals.size() == 4_ul);
        auto chord = C(4) + pattern;
        expect(chord[3].get_pitch() == D.get_pitch());
    };


    "analyze major triad"_test = [] {
        auto chord = C(4) + major_triad;
        auto result = chord.analyze();
        expect(!result.empty());
        expect(result[0].root.get_pitch() == C.get_pitch());
        expect(result[0].quality == ""s);
    };

    "analyze minor triad"_test = [] {
        auto chord = A(4) + minor_triad;
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == A.get_pitch() && a.quality == "m") {
                found = true;
                break;
            }
        }
        expect(found) << "should find Am";
    };

    "analyze diminished triad"_test = [] {
        auto chord = B(4) + diminished_triad;
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == B.get_pitch() && a.quality == "dim") {
                found = true;
                break;
            }
        }
        expect(found) << "should find Bdim";
    };

    "analyze augmented triad"_test = [] {
        auto chord = C(4) + augmented_triad;
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == C.get_pitch() && a.quality == "aug") {
                found = true;
                break;
            }
        }
        expect(found) << "should find Caug";
    };

    "analyze dom7"_test = [] {
        auto chord = G(4) + dom7;
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == G.get_pitch() && a.quality == "7") {
                found = true;
                break;
            }
        }
        expect(found) << "should find G7";
    };

    "analyze maj7"_test = [] {
        auto chord = C(4) + maj7;
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == C.get_pitch() && a.quality == "maj7") {
                found = true;
                break;
            }
        }
        expect(found) << "should find Cmaj7";
    };

    "analyze with specified root"_test = [] {
        auto chord = C(4) + major_triad;
        auto result = chord.analyze(C);
        expect(result.has_value());
        expect(result->root.get_pitch() == C.get_pitch());
        expect(result->quality == ""s);
    };

    "analyze with wrong root returns nullopt"_test = [] {
        auto chord = C(4) + major_triad;
        auto result = chord.analyze(Fs);
        expect(!result.has_value());
    };


    "analyze chord with omitted 5th"_test = [] {
        auto chord = C(4) + dom7.omit<2>();
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == C.get_pitch() && a.quality == "7") {
                found = true;
                expect(!a.omissions.empty()) << "should have omissions";
                break;
            }
        }
        expect(found) << "should find C7(no5)";
    };


    "analyze first inversion"_test = [] {
        auto chord = C(4) + major_triad.inversion<1>();
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == C.get_pitch() && a.quality == "") {
                found = true;
                expect(a.bass.has_value()) << "should have bass note";
                expect(a.inversion != 0_i) << "should detect inversion";
                break;
            }
        }
        expect(found) << "should find C/E";
    };

    "analyze second inversion"_test = [] {
        auto chord = C(4) + major_triad.inversion<2>();
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.root.get_pitch() == C.get_pitch() && a.quality == "") {
                found = true;
                expect(a.bass.has_value()) << "should have bass note";
                break;
            }
        }
        expect(found) << "should find C/G";
    };


    "slash chord construction"_test = [] {
        auto chord = C(4) + major_triad;
        auto slash = chord / G(3);
        expect(slash.bass.get_pitch() == G.get_pitch());
        expect(slash.size() == 3_ul);
    };

    "slash chord all_notes"_test = [] {
        auto chord = C(4) + major_triad;
        auto slash = chord / G(3);
        auto all = slash.all_notes();
        expect(all.size() == 4_ul);
        expect(all[0].get_pitch() == G.get_pitch());
    };


    "roman numeral I in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = key.chord_on<1, 3>();
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "I") {
                found = true;
                expect(da.deg.num == 1_i);
                break;
            }
        }
        expect(found) << "should find I";
    };

    "roman numeral ii in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = key.chord_on<2, 3>();
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "ii") {
                found = true;
                expect(da.deg.num == 2_i);
                break;
            }
        }
        expect(found) << "should find ii";
    };

    "roman numeral V in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = key.chord_on<5, 3>();
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "V") {
                found = true;
                expect(da.deg.num == 5_i);
                break;
            }
        }
        expect(found) << "should find V";
    };

    "roman numeral viidim in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = key.chord_on<7, 3>();
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            auto rn = da.roman_numeral;
            if (rn.find("vii") != std::string::npos) {
                found = true;
                expect(da.deg.num == 7_i);
                break;
            }
        }
        expect(found) << "should find vii";
    };

    "roman numeral Imaj7 in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = key.chord_on<1, 4>();
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "Imaj7") {
                found = true;
                break;
            }
        }
        expect(found) << "should find Imaj7";
    };

    "roman numeral IVmaj7 in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = key.chord_on<4, 4>();
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "IVmaj7") {
                found = true;
                break;
            }
        }
        expect(found) << "should find IVmaj7";
    };

    "roman numeral V7 in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = key.chord_on<5, 4>();
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "V7") {
                found = true;
                break;
            }
        }
        expect(found) << "should find V7";
    };

    "roman numeral chromatic bVI in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = Ab(3) + major_triad;
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "#V") {
                found = true;
                break;
            }
        }
        expect(found) << "should find bVI";
    };

    "roman numeral chromatic bVII7 in C major"_test = [] {
        auto key = C(4) + major;
        auto chord = Bb(3) + dom7;
        auto result = chord.analyze(key);
        expect(!result.empty());
        bool found = false;
        for (const auto &da : result) {
            if (da.roman_numeral == "#VI7") {
                found = true;
                break;
            }
        }
        expect(found) << "should find bVII7";
    };


    "chord simplify"_test = [] {
        auto chord = C(4) + major_triad;
        auto s = chord.simplify();
        expect(s[0].get_pitch() == C.get_pitch());
        expect(s[1].get_pitch() == E.get_pitch());
        expect(s[2].get_pitch() == G.get_pitch());
    };


    "chord_analysis str"_test = [] {
        auto chord = C(4) + major_triad;
        auto result = chord.analyze();
        expect(!result.empty());
        auto str = result[0].str();
        expect(str.find("C") != std::string::npos);
    };

    "analysis_result str"_test = [] {
        auto chord = C(4) + major_triad;
        auto result = chord.analyze();
        auto str = result.str();
        expect(!str.empty());
        expect(str != "?"s);
    };


    "chord std::format"_test = [] {
        auto chord = C(4) + major_triad;
        auto str = std::format("{}", chord);
        expect(str.find("C4") != std::string::npos);
        expect(str.find("E4") != std::string::npos);
        expect(str.find("G4") != std::string::npos);
    };

    "analysis_result std::format"_test = [] {
        auto chord = C(4) + major_triad;
        auto result = chord.analyze();
        auto str = std::format("{}", result);
        expect(!str.empty());
    };


    "power chord"_test = [] {
        auto chord = C(4) + power_chord;
        expect(chord.size() == 2_ul);
        expect(chord[0].get_pitch() == C.get_pitch());
        expect(chord[1].get_pitch() == G.get_pitch());
        auto result = chord.analyze();
        expect(!result.empty());
        bool found = false;
        for (const auto &a : result) {
            if (a.quality == "5") {
                found = true;
                break;
            }
        }
        expect(found) << "should find power chord (5)";
    };
}
