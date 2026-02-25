#include <boost/ut.hpp>
#include <musicpp/scales.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::notes;
    using namespace musicpp::intervals;
    using namespace musicpp::scale_patterns;
    using namespace musicpp::chord_patterns;
    using namespace std::literals;


    "C major scale construction"_test = [] {
        auto s = C(4) + major;
        expect(s.size() == 7_ul);
        expect(s[0].get_pitch() == C.get_pitch());
        expect(s[1].get_pitch() == D.get_pitch());
        expect(s[2].get_pitch() == E.get_pitch());
        expect(s[3].get_pitch() == F.get_pitch());
        expect(s[4].get_pitch() == G.get_pitch());
        expect(s[5].get_pitch() == A.get_pitch());
        expect(s[6].get_pitch() == B.get_pitch());
    };

    "A natural minor scale construction"_test = [] {
        auto s = A(3) + natural_minor;
        expect(s[0].get_pitch() == A.get_pitch());
        expect(s[1].get_pitch() == B.get_pitch());
        expect(s[2].get_pitch() == C.get_pitch());
        expect(s[3].get_pitch() == D.get_pitch());
        expect(s[4].get_pitch() == E.get_pitch());
        expect(s[5].get_pitch() == F.get_pitch());
        expect(s[6].get_pitch() == G.get_pitch());
    };

    "G major scale construction"_test = [] {
        auto s = G(4) + major;
        expect(s[0].get_pitch() == G.get_pitch());
        expect(s[1].get_pitch() == A.get_pitch());
        expect(s[2].get_pitch() == B.get_pitch());
        expect(s[3].get_pitch() == C.get_pitch());
        expect(s[4].get_pitch() == D.get_pitch());
        expect(s[5].get_pitch() == E.get_pitch());
        expect(s[6].get_pitch() == Fs.get_pitch());
    };

    "F major scale construction"_test = [] {
        auto s = F(4) + major;
        expect(s[0].get_pitch() == F.get_pitch());
        expect(s[6].get_pitch() == E.get_pitch());
        expect(s[3].get_pitch() == Bb.get_pitch());
    };


    "D dorian mode"_test = [] {
        auto s = D(4) + dorian;
        expect(s[0].get_pitch() == D.get_pitch());
        expect(s[2].get_pitch() == F.get_pitch());
        expect(s[5].get_pitch() == B.get_pitch());
    };

    "F lydian mode"_test = [] {
        auto s = F(4) + lydian;
        expect(s[0].get_pitch() == F.get_pitch());
        expect(s[3].get_pitch() == B.get_pitch());
    };

    "G mixolydian mode"_test = [] {
        auto s = G(4) + mixolydian;
        expect(s[0].get_pitch() == G.get_pitch());
        expect(s[6].get_pitch() == F.get_pitch());
    };

    "B locrian mode"_test = [] {
        auto s = B(3) + locrian;
        expect(s[0].get_pitch() == B.get_pitch());
        expect(s[1].get_pitch() == C.get_pitch());
        expect(s[4].get_pitch() == F.get_pitch());
    };


    "A harmonic minor scale"_test = [] {
        auto s = A(3) + harmonic_minor;
        expect(s[0].get_pitch() == A.get_pitch());
        expect(s[5].get_pitch() == F.get_pitch());
        expect(s[6].get_pitch() == Gs.get_pitch());
    };

    "A melodic minor scale"_test = [] {
        auto s = A(3) + melodic_minor;
        expect(s[0].get_pitch() == A.get_pitch());
        expect(s[2].get_pitch() == C.get_pitch());
        expect(s[5].get_pitch() == Fs.get_pitch());
        expect(s[6].get_pitch() == Gs.get_pitch());
    };


    "C major pentatonic"_test = [] {
        auto s = C(4) + major_pentatonic;
        expect(s.size() == 5_ul);
        expect(s[0].get_pitch() == C.get_pitch());
        expect(s[1].get_pitch() == D.get_pitch());
        expect(s[2].get_pitch() == E.get_pitch());
        expect(s[3].get_pitch() == G.get_pitch());
        expect(s[4].get_pitch() == A.get_pitch());
    };

    "A minor pentatonic"_test = [] {
        auto s = A(3) + minor_pentatonic;
        expect(s.size() == 5_ul);
        expect(s[0].get_pitch() == A.get_pitch());
        expect(s[1].get_pitch() == C.get_pitch());
        expect(s[2].get_pitch() == D.get_pitch());
        expect(s[3].get_pitch() == E.get_pitch());
        expect(s[4].get_pitch() == G.get_pitch());
    };

    "C blues scale"_test = [] {
        auto s = C(4) + blues;
        expect(s.size() == 6_ul);
        expect(s[0].get_pitch() == C.get_pitch());
    };

    "C whole tone scale"_test = [] {
        auto s = C(4) + whole_tone;
        expect(s.size() == 6_ul);
        for (std::size_t i = 1; i < 6; ++i) {
            int diff = (s[i].get_pitch() - s[i - 1].get_pitch() + 12) % 12;
            expect(diff == 2_i) << "step " << i;
        }
    };


    "scale contains note"_test = [] {
        auto key = C(4) + major;
        expect(key.contains(C(4)));
        expect(key.contains(D(4)));
        expect(key.contains(E(4)));
        expect(key.contains(F(4)));
        expect(key.contains(G(4)));
        expect(key.contains(A(4)));
        expect(key.contains(B(4)));
        expect(!key.contains(Fs(4)));
        expect(!key.contains(Bb(4)));
    };

    "scale contains_enharmonic"_test = [] {
        auto key = Fs(4) + major;
        expect(key.contains_enharmonic(Fs(4)));
    };


    "scale degree_of"_test = [] {
        auto key = C(4) + major;
        expect(key.degree_of(C(4)).num == 1_i);
        expect(key.degree_of(D(4)).num == 2_i);
        expect(key.degree_of(E(4)).num == 3_i);
        expect(key.degree_of(F(4)).num == 4_i);
        expect(key.degree_of(G(4)).num == 5_i);
        expect(key.degree_of(A(4)).num == 6_i);
        expect(key.degree_of(B(4)).num == 7_i);
        expect(key.degree_of(Fs(4)).num == 0_i);
    };


    "is_diatonic note"_test = [] {
        auto key = C(4) + major;
        expect(key.is_diatonic(C(4)));
        expect(key.is_diatonic(G(4)));
        expect(!key.is_diatonic(Fs(4)));
        expect(!key.is_diatonic(Bb(4)));
    };

    "is_diatonic chord"_test = [] {
        auto key = C(4) + major;
        auto dm = D(4) + minor_triad;
        expect(key.is_diatonic(dm));

        auto ab_maj = Ab(3) + major_triad;
        expect(!key.is_diatonic(ab_maj));

        auto g_maj = G(4) + major_triad;
        expect(key.is_diatonic(g_maj));
    };


    "diatonic triads in C major"_test = [] {
        auto key = C(4) + major;

        auto I = key.chord_on<1, 3>();
        expect(I[0].get_pitch() == C.get_pitch());
        expect(I[1].get_pitch() == E.get_pitch());
        expect(I[2].get_pitch() == G.get_pitch());

        auto ii = key.chord_on<2, 3>();
        expect(ii[0].get_pitch() == D.get_pitch());
        expect(ii[1].get_pitch() == F.get_pitch());
        expect(ii[2].get_pitch() == A.get_pitch());

        auto V = key.chord_on<5, 3>();
        expect(V[0].get_pitch() == G.get_pitch());
        expect(V[1].get_pitch() == B.get_pitch());
        expect(V[2].get_pitch() == D.get_pitch());
    };

    "diatonic seventh chords in C major"_test = [] {
        auto key = C(4) + major;

        auto Imaj7 = key.chord_on<1, 4>();
        expect(Imaj7[0].get_pitch() == C.get_pitch());
        expect(Imaj7[1].get_pitch() == E.get_pitch());
        expect(Imaj7[2].get_pitch() == G.get_pitch());
        expect(Imaj7[3].get_pitch() == B.get_pitch());

        auto V7 = key.chord_on<5, 4>();
        expect(V7[0].get_pitch() == G.get_pitch());
        expect(V7[1].get_pitch() == B.get_pitch());
        expect(V7[2].get_pitch() == D.get_pitch());
        expect(V7[3].get_pitch() == F.get_pitch());
    };


    "scale stores root note"_test = [] {
        auto s = C(4) + major;
        expect(s.root == C(4));

        auto s2 = A(3) + natural_minor;
        expect(s2.root == A(3));
    };


    "scale simplify"_test = [] {
        auto s = Gb(4) + major;
        auto simplified = s.simplify();
        expect(simplified.root.get_pitch() == Gb.get_pitch());
    };


    "chromatic scale has 12 notes"_test = [] {
        auto s = C(4) + chromatic;
        expect(s.size() == 12_ul);
    };


    "bebop dominant has 8 notes"_test = [] {
        auto s = C(4) + bebop_dominant;
        expect(s.size() == 8_ul);
    };

    "bebop major has 8 notes"_test = [] {
        auto s = C(4) + bebop_major;
        expect(s.size() == 8_ul);
    };


    "scale std::format"_test = [] {
        auto s = C(4) + major;
        auto str = std::format("{}", s);
        expect(str.find("C4") != std::string::npos);
        expect(str.find("D4") != std::string::npos);
        expect(str.find("E4") != std::string::npos);
    };
}
