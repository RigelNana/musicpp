#include <boost/ut.hpp>
#include <musicpp/duration.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::durations;
    using namespace std::literals;


    "duration default"_test = [] {
        duration d;
        expect(d.num == 1_i);
        expect(d.den == 4_i);
    };

    "duration normalizes gcd"_test = [] {
        duration d{2, 8};
        expect(d.num == 1_i);
        expect(d.den == 4_i);
    };

    "duration normalizes negative denominator"_test = [] {
        duration d{1, -4};
        expect(d.num == -1_i);
        expect(d.den == 4_i);
    };


    "predefined durations"_test = [] {
        expect(whole.num == 1_i);
        expect(whole.den == 1_i);
        expect(half.num == 1_i);
        expect(half.den == 2_i);
        expect(quarter.num == 1_i);
        expect(quarter.den == 4_i);
        expect(eighth.num == 1_i);
        expect(eighth.den == 8_i);
        expect(sixteenth.num == 1_i);
        expect(sixteenth.den == 16_i);
    };

    "short aliases"_test = [] {
        expect(w == whole);
        expect(h == half);
        expect(q == quarter);
    };


    "duration addition"_test = [] {
        auto result = quarter + quarter;
        expect(result == half);
    };

    "duration addition three quarters"_test = [] {
        auto result = quarter + quarter + quarter;
        expect(result == duration{3, 4});
    };

    "duration multiply by int"_test = [] {
        auto result = quarter * 2;
        expect(result == half);
    };

    "int multiply duration"_test = [] {
        auto result = 4 * quarter;
        expect(result == whole);
    };


    "duration equality"_test = [] {
        expect(duration{1, 4} == duration{2, 8});
        expect(duration{1, 2} == duration{2, 4});
    };

    "duration ordering"_test = [] {
        expect(eighth < quarter);
        expect(quarter < half);
        expect(half < whole);
        expect(!(whole < half));
    };


    "dotted duration"_test = [] {
        auto dq = quarter.dotted();
        expect(dq == duration{3, 8});
    };

    "double dotted duration"_test = [] {
        auto ddq = quarter.double_dotted();
        expect(ddq == duration{7, 16});
    };

    "triplet duration"_test = [] {
        auto tq = quarter.triplet();
        expect(tq == duration{1, 6});
    };


    "beats default (quarter=1)"_test = [] {
        expect(quarter.beats() == 1.0_d);
        expect(half.beats() == 2.0_d);
        expect(whole.beats() == 4.0_d);
        expect(eighth.beats() == 0.5_d);
    };

    "beats with custom beat unit"_test = [] {
        expect(eighth.beats(8) == 1.0_d);
        expect(quarter.beats(8) == 2.0_d);
    };


    "duration str"_test = [] {
        expect(whole.str() == "w"s);
        expect(half.str() == "h"s);
        expect(quarter.str() == "q"s);
        expect(eighth.str() == "8th"s);
        expect(sixteenth.str() == "16th"s);
    };

    "dotted duration str"_test = [] {
        expect(half.dotted().str() == "h."s);
        expect(quarter.dotted().str() == "q."s);
    };

    "arbitrary duration str"_test = [] {
        duration d{5, 8};
        expect(d.str() == "5/8"s);
    };
}
