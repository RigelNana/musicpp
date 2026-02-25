#include <boost/ut.hpp>
#include <musicpp/progressions.hpp>
#include <musicpp/timing.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::notes;
    using namespace musicpp::intervals;
    using namespace musicpp::durations;
    using namespace musicpp::chord_patterns;
    using namespace musicpp::scale_patterns;
    using namespace std::literals;


    "step creates progression_step"_test = [] {
        auto s = step<1>(major_triad, q);
        expect(s.dur == quarter);
    };

    "step with flat degree"_test = [] {
        auto s = step<b(7)>(major_triad, h);
        expect(s.dur == half);
    };

    "step with sharp degree"_test = [] {
        auto st = step<s(4)>(major_triad, w);
        expect(st.dur == whole);
    };


    "nominal creates chromatic step"_test = [] {
        auto ns = nominal<Ab(3)>(major_triad, h);
        expect(ns.dur == half);
        expect(ns.root == Ab(3));
    };


    "prog_rest construction"_test = [] {
        auto r = prog_rest(q);
        expect(r.is_rest);
        expect(r.dur == quarter);
    };


    "two steps make progression"_test = [] {
        auto prog = step<1>(major_triad, q)
                  | step<5>(major_triad, q);
        expect(prog.length == 2_ul);
    };

    "progression append step"_test = [] {
        auto prog = step<1>(major_triad, q)
                  | step<4>(major_triad, q)
                  | step<5>(major_triad, q);
        expect(prog.length == 3_ul);
    };

    "progression concatenation"_test = [] {
        auto a = step<1>(major_triad, q) | step<4>(major_triad, q);
        auto b = step<5>(major_triad, q) | step<1>(major_triad, q);
        auto c = a | b;
        expect(c.length == 4_ul);
    };


    "realize I-V in C major"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, h)
                  | step<5>(major_triad, h);
        auto seq = prog.realize(key);
        expect(seq.length == 2_ul);
        expect(seq.total_duration() == whole);
        auto names = seq.names();
        expect(names.find("C") != std::string::npos);
        expect(names.find("G") != std::string::npos);
    };

    "realize I-IV-V-I in C major"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, q)
                  | step<4>(major_triad, q)
                  | step<5>(major_triad, q)
                  | step<1>(major_triad, q);
        auto seq = prog.realize(key);
        expect(seq.total_duration() == whole);
        auto names = seq.names();
        expect(names.find("C") != std::string::npos);
        expect(names.find("F") != std::string::npos);
        expect(names.find("G") != std::string::npos);
    };

    "realize in different keys"_test = [] {
        auto prog = step<1>(major_triad, h)
                  | step<5>(major_triad, h);

        auto c_key = C(4) + major;
        auto c_names = prog.realize(c_key).names();
        expect(c_names.find("C") != std::string::npos);
        expect(c_names.find("G") != std::string::npos);

        auto g_key = G(4) + major;
        auto g_names = prog.realize(g_key).names();
        expect(g_names.find("G") != std::string::npos);
        expect(g_names.find("D") != std::string::npos);
    };


    "realize ii-V-I jazz"_test = [] {
        auto key = C(4) + major;
        auto prog = step<2>(min7, w)
                  | step<5>(dom7, w)
                  | step<1>(maj7, w);
        auto seq = prog.realize(key);
        auto names = seq.names();
        expect(names.find("Dm7") != std::string::npos);
        expect(names.find("G7") != std::string::npos);
        expect(names.find("Cmaj7") != std::string::npos);
    };


    "realize bVII in C major"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, h)
                  | step<b(7)>(major_triad, h);
        auto seq = prog.realize(key);
        auto names = seq.names();
        expect(names.find("C") != std::string::npos);
        expect(names.find("Bb") != std::string::npos);
    };

    "realize bVI in C major"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, h)
                  | step<b(6)>(major_triad, h);
        auto seq = prog.realize(key);
        auto names = seq.names();
        expect(names.find("C") != std::string::npos);
        expect(names.find("Ab") != std::string::npos);
    };


    "realize #IV dim in C major"_test = [] {
        auto key = C(4) + major;
        auto st = step<s(4)>(diminished_triad, h);
        auto prog = step<1>(major_triad, h) | st;
        auto seq = prog.realize(key);
        auto names = seq.names();
        expect(names.find("C") != std::string::npos);
        expect(names.find("F#") != std::string::npos);
    };


    "realize with nominal chromatic"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, h)
                  | nominal<Eb(4)>(major_triad, h);
        auto seq = prog.realize(key);
        auto names = seq.names();
        expect(names.find("C") != std::string::npos);
        expect(names.find("Eb") != std::string::npos);
    };


    "realize with rest"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, h)
                  | prog_rest(h);
        auto seq = prog.realize(key);
        expect(seq.total_duration() == whole);
    };


    "progression str helper"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, h)
                  | step<5>(major_triad, h);
        auto s = prog.str(key);
        expect(s.find("C") != std::string::npos);
        expect(s.find("G") != std::string::npos);
    };

    "progression roman helper"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, h)
                  | step<5>(major_triad, h);
        auto r = prog.roman(key);
        expect(r.find("I") != std::string::npos);
        expect(r.find("V") != std::string::npos);
    };


    "realized progression walk"_test = [] {
        auto key = C(4) + major;
        auto prog = step<1>(major_triad, w)
                  | step<5>(major_triad, w);
        auto seq = prog.realize(key);
        int count = 0;
        seq.walk(musicpp::time_signatures::common, [&](const auto &, auto) {
            ++count;
        });
        expect(count == 2_i);
    };
}
