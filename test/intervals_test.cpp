#include <boost/ut.hpp>
#include <musicpp/intervals.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::intervals;

    "interval basic properties"_test = [] {
        expect(P1.fifths == 0_i);
        expect(P1.octaves == 0_i);
        expect(P5.fifths == 1_i);
        expect(P5.octaves == 0_i);
        expect(P4.fifths == -1_i);
        expect(P4.octaves == 1_i);
        expect(P8.fifths == 0_i);
        expect(P8.octaves == 1_i);
    };

    "interval addition"_test = [] {
        auto two_fifths = P5 + P5;
        expect(two_fifths.fifths == 2_i);
        expect(two_fifths.octaves == 0_i);

        auto p4_p5 = P4 + P5;
        expect(p4_p5.fifths == P8.fifths);
        expect(p4_p5.octaves == P8.octaves);

        auto m3_M3 = M3 + m3;
        expect(m3_M3.fifths == P5.fifths);
        expect(m3_M3.octaves == P5.octaves);

        auto M3_m3 = m3 + M3;
        expect(M3_m3.fifths == P5.fifths);
        expect(M3_m3.octaves == P5.octaves);

        auto p1_m7 = P1 + m7;
        expect(p1_m7.fifths == m7.fifths);
        expect(p1_m7.octaves == m7.octaves);
    };

    "interval octave offset"_test = [] {
        auto m2_up = m2(1);
        expect(m2_up.fifths == m9.fifths);
        expect(m2_up.octaves == m9.octaves);

        auto M2_up = M2(1);
        expect(M2_up.fifths == M9.fifths);
        expect(M2_up.octaves == M9.octaves);

        auto P4_up = P4(1);
        expect(P4_up.fifths == P11.fifths);
        expect(P4_up.octaves == P11.octaves);

        auto M6_up = M6(1);
        expect(M6_up.fifths == M13.fifths);
        expect(M6_up.octaves == M13.octaves);

        auto P8_down = P8(-1);
        expect(P8_down.fifths == P1.fifths);
        expect(P8_down.octaves == P1.octaves);
    };

    "compound intervals consistency"_test = [] {
        expect(m9.fifths == m2.fifths);
        expect(M9.fifths == M2.fifths);
        expect(P11.fifths == P4.fifths);
        expect(M13.fifths == M6.fifths);
        expect(m13.fifths == m6.fifths);
        expect(A9.fifths == A2.fifths);
        expect(A11.fifths == A4.fifths);

        expect(m9.octaves == static_cast<int8_t>(m2.octaves + 1));
        expect(M9.octaves == static_cast<int8_t>(M2.octaves + 1));
        expect(P11.octaves == static_cast<int8_t>(P4.octaves + 1));
    };

    "tritone intervals"_test = [] {
        expect(A4.is_enharmonic(d5));
        expect(A4.semitones() == 6_i);
        expect(d5.semitones() == 6_i);
    };

    "interval subtraction"_test = [] {
        auto result = P5 - M3;
        expect(result.fifths == m3.fifths);
        expect(result.octaves == m3.octaves);

        auto p4 = P8 - P5;
        expect(p4.fifths == P4.fifths);
        expect(p4.octaves == P4.octaves);
    };

    "interval negation"_test = [] {
        auto neg_P5 = -P5;
        expect(neg_P5.fifths == -1_i);
        expect(neg_P5.octaves == 0_i);

        auto zero = P5 + (-P5);
        expect(zero.fifths == P1.fifths);
        expect(zero.octaves == P1.octaves);
    };



    "interval semitones"_test = [] {
        expect(P1.semitones() == 0_i);
        expect(m2.semitones() == 1_i);
        expect(M2.semitones() == 2_i);
        expect(m3.semitones() == 3_i);
        expect(M3.semitones() == 4_i);
        expect(P4.semitones() == 5_i);
        expect(P5.semitones() == 7_i);
        expect(m7.semitones() == 10_i);
        expect(M7.semitones() == 11_i);
    };

    "interval is_enharmonic"_test = [] {
        expect(A4.is_enharmonic(d5));
        expect(A1.is_enharmonic(m2));
        expect(!P4.is_enharmonic(P5));
    };

    "interval comparison"_test = [] {
        expect(P1 == P1);
        expect(P1 != P5);
        expect(P1 < P5);
    };
}
