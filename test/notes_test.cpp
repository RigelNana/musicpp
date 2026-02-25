#include <boost/ut.hpp>
#include <format>
#include <sstream>
#include <musicpp/notes.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::notes;
    using namespace musicpp::intervals;
    using namespace std::literals;

    "note basic properties"_test = [] {
        expect(C.get_fifth() == 0_i);
        expect(C.get_octave() == 0_i);
        expect(G.get_fifth() == 1_i);
        expect(G.get_octave() == 0_i);
        expect(D.get_fifth() == 2_i);
        expect(D.get_octave() == -1_i);
        expect(F.get_fifth() == -1_i);
        expect(F.get_octave() == 1_i);
    };

    "note pitch class"_test = [] {
        expect(C.get_pitch() == 0_i);
        expect(D.get_pitch() == 2_i);
        expect(E.get_pitch() == 4_i);
        expect(F.get_pitch() == 5_i);
        expect(G.get_pitch() == 7_i);
        expect(A.get_pitch() == 9_i);
        expect(B.get_pitch() == 11_i);
        expect(Fs.get_pitch() == 6_i);
        expect(Bb.get_pitch() == 10_i);
        expect(Eb.get_pitch() == 3_i);
        expect(Ab.get_pitch() == 8_i);
    };

    "note midi pitch"_test = [] {
        expect(C.get_midi_pitch() == 12_i);
        auto C4 = C(4);
        expect(C4.get_midi_pitch() == 60_i);
        auto A4 = A(4);
        expect(A4.get_midi_pitch() == 69_i);
    };

    "note octave operator"_test = [] {
        auto C3 = C(3);
        auto C4 = C(4);
        auto C5 = C(5);
        expect(C4.get_midi_pitch() - C3.get_midi_pitch() == 12_i);
        expect(C5.get_midi_pitch() - C4.get_midi_pitch() == 12_i);
    };

    "note + interval"_test = [] {
        auto result = C + M3;
        expect(result.get_fifth() == E.get_fifth());
        expect(result.get_pitch() == E.get_pitch());

        auto g = C + P5;
        expect(g.get_fifth() == G.get_fifth());
        expect(g.get_pitch() == G.get_pitch());

        auto eb = C + m3;
        expect(eb.get_pitch() == Eb.get_pitch());

        auto b = C + M7;
        expect(b.get_pitch() == B.get_pitch());

        auto f = D + m3;
        expect(f.get_pitch() == F.get_pitch());
    };

    "enharmonic equivalence"_test = [] {
        expect(Fs.is_enharmonic(Gb));
        expect(Cs.is_enharmonic(Db));
        expect(Ds.is_enharmonic(Eb));
        expect(C.is_enharmonic(C));
        expect(!C.is_enharmonic(D));
    };

    "note comparison"_test = [] {
        expect(C == C);
        expect(C != G);
        auto C4 = C(4);
        auto C5 = C(5);
        expect(C4 < C5);
        expect(C5 > C4);
    };

    "note string representation"_test = [] {
        auto C4 = C(4);
        expect(C4.str() == "C4"s);

        auto A4 = A(4);
        expect(A4.str() == "A4"s);

        auto Fs4 = Fs(4);
        expect(Fs4.str() == "F#4"s);

        auto Bb3 = Bb(3);
        expect(Bb3.str() == "Bb3"s);

        auto Eb4 = Eb(4);
        expect(Eb4.str() == "Eb4"s);
    };

    "note - interval (transpose down)"_test = [] {
        auto result = E - M3;
        expect(result.get_fifth() == C.get_fifth());
        expect(result.get_pitch() == C.get_pitch());

        auto c = G - P5;
        expect(c.get_fifth() == C.get_fifth());

        auto roundtrip = (C + M3) - M3;
        expect(roundtrip == C);
    };

    "note - note (interval between notes)"_test = [] {
        auto iv = E - C;
        expect(iv.fifths == M3.fifths);
        expect(iv.octaves == M3.octaves);

        auto p5 = G - C;
        expect(p5.fifths == P5.fifths);
        expect(p5.octaves == P5.octaves);

        auto octave = C(5) - C(4);
        expect(octave.fifths == P8.fifths);
        expect(octave.octaves == P8.octaves);
    };

    "note std::format"_test = [] {
        auto C4 = C(4);
        expect(std::format("{}", C4) == "C4"s);
        expect(std::format("note: {}", A(4)) == "note: A4"s);
    };

    "note operator<<"_test = [] {
        std::ostringstream oss;
        oss << C(4);
        expect(oss.str() == "C4"s);
    };
}