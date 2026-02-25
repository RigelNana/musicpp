#include <boost/ut.hpp>
#include <musicpp/timing.hpp>
#include <musicpp/scales.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::notes;
    using namespace musicpp::intervals;
    using namespace musicpp::durations;
    using namespace musicpp::chord_patterns;
    using namespace musicpp::scale_patterns;
    using namespace musicpp::time_signatures;
    using namespace musicpp::tempos;
    using namespace std::literals;


    "time_signature default"_test = [] {
        time_signature ts;
        expect(ts.beats == 4_i);
        expect(ts.beat_unit == 4_i);
    };

    "time_signature beat_duration"_test = [] {
        expect(common.beat_duration() == quarter);
        expect(six_eight.beat_duration() == eighth);
        expect(cut.beat_duration() == half);
    };

    "time_signature bar_duration"_test = [] {
        expect(common.bar_duration() == whole);
        expect(waltz.bar_duration() == duration{3, 4});
        expect(six_eight.bar_duration() == duration{6, 8});
    };


    "time_signature is_simple"_test = [] {
        expect(common.is_simple());
        expect(waltz.is_simple());
        expect(cut.is_simple());
        expect(!six_eight.is_simple());
        expect(!seven_eight.is_simple());
    };

    "time_signature is_compound"_test = [] {
        expect(six_eight.is_compound());
        expect(nine_eight.is_compound());
        expect(twelve_eight.is_compound());
        expect(!common.is_compound());
        expect(!waltz.is_compound());
    };

    "time_signature is_irregular"_test = [] {
        expect(five_four.is_irregular());
        expect(seven_eight.is_irregular());
        expect(!common.is_irregular());
        expect(!six_eight.is_irregular());
    };


    "time_signature pulse_count"_test = [] {
        expect(common.pulse_count() == 4_i);
        expect(waltz.pulse_count() == 3_i);
        expect(six_eight.pulse_count() == 2_i);
        expect(nine_eight.pulse_count() == 3_i);
        expect(twelve_eight.pulse_count() == 4_i);
    };

    "time_signature pulse_duration"_test = [] {
        expect(common.pulse_duration() == quarter);
        expect(six_eight.pulse_duration() == duration{3, 8});
    };


    "fills one bar"_test = [] {
        expect(common.fills(whole));
        expect(waltz.fills(duration{3, 4}));
        expect(!common.fills(half));
    };

    "fills multiple bars"_test = [] {
        auto two_bars = whole * 2;
        expect(common.fills(two_bars, 2));
    };

    "bar_count"_test = [] {
        expect(common.bar_count(whole) == 1_i);
        expect(common.bar_count(whole * 2) == 2_i);
        expect(common.bar_count(half) == 0_i);
    };

    "remainder"_test = [] {
        auto dur = whole + half;
        auto rem = common.remainder(dur);
        expect(rem == half);
    };


    "time_signature str"_test = [] {
        expect(common.str() == "4/4"s);
        expect(waltz.str() == "3/4"s);
        expect(six_eight.str() == "6/8"s);
    };

    "time_signature std::format"_test = [] {
        auto s = std::format("{}", common);
        expect(s == "4/4"s);
    };


    "predefined time signatures"_test = [] {
        expect(common.beats == 4_i);
        expect(cut.beats == 2_i);
        expect(waltz.beats == 3_i);
        expect(six_eight.beats == 6_i);
        expect(five_four.beats == 5_i);
        expect(seven_eight.beats == 7_i);
    };


    "tempo default"_test = [] {
        tempo t;
        expect(t.bpm == 120.0_d);
        expect(t.beat == quarter);
    };

    "tempo from bpm"_test = [] {
        tempo t{140.0};
        expect(t.bpm == 140.0_d);
        expect(t.beat == quarter);
    };

    "tempo seconds_per_beat"_test = [] {
        tempo t{120.0};
        expect(t.seconds_per_beat() == 0.5_d);
    };

    "tempo seconds for duration"_test = [] {
        tempo t{120.0};
        expect(t.seconds(quarter) == 0.5_d);
        expect(t.seconds(half) == 1.0_d);
        expect(t.seconds(whole) == 2.0_d);
    };

    "tempo ms"_test = [] {
        tempo t{120.0};
        expect(t.ms(quarter) == 500.0_d);
    };

    "tempo bar_seconds"_test = [] {
        tempo t{120.0};
        expect(t.bar_seconds(common) == 2.0_d);
        expect(t.bar_seconds(common, 2) == 4.0_d);
    };


    "predefined tempos"_test = [] {
        expect(largo.bpm == 50.0_d);
        expect(allegro.bpm == 140.0_d);
        expect(presto.bpm == 190.0_d);
    };


    "metric_position is_downbeat"_test = [] {
        metric_position p{0, {0, 1}};
        expect(p.is_downbeat());
        metric_position p2{1, quarter};
        expect(!p2.is_downbeat());
    };

    "metric_position is_on_beat"_test = [] {
        metric_position p{0, {0, 1}};
        expect(p.is_on_beat(common));

        metric_position p2{0, quarter};
        expect(p2.is_on_beat(common));

        metric_position p3{0, eighth};
        expect(!p3.is_on_beat(common));
    };

    "metric_position beat_index"_test = [] {
        metric_position p{0, {0, 1}};
        expect(p.beat_index(common) == 0_i);

        metric_position p2{0, quarter};
        expect(p2.beat_index(common) == 1_i);

        metric_position p3{0, half};
        expect(p3.beat_index(common) == 2_i);
    };


    "melody walk visits all events"_test = [] {
        auto m = C(4) * q | D(4) * q | E(4) * q | F(4) * q;
        int count = 0;
        m.walk(common, [&](const auto &, auto) { ++count; });
        expect(count == 4_i);
    };

    "melody walk tracks bars"_test = [] {
        auto m = C(4) * q | D(4) * q | E(4) * q | F(4) * q
               | G(4) * q | A(4) * q | B(4) * q | C(5) * q;
        int max_bar = 0;
        m.walk(common, [&](const auto &, auto pos) {
            if (pos.bar > max_bar) max_bar = pos.bar;
        });
        expect(max_bar == 1_i);
    };

    "melody walk downbeats"_test = [] {
        auto m = C(4) * q | D(4) * q | E(4) * q | F(4) * q
               | G(4) * q | A(4) * q | B(4) * q | C(5) * q;
        int downbeats = 0;
        m.walk(common, [&](const auto &, auto pos) {
            if (pos.is_downbeat()) ++downbeats;
        });
        expect(downbeats == 2_i);
    };


    "chord_sequence walk visits all"_test = [] {
        auto seq = (C(4) + major_triad) * h
                 | (G(3) + major_triad) * h;
        int count = 0;
        seq.walk(common, [&](const auto &, auto) { ++count; });
        expect(count == 2_i);
    };

    "chord_sequence walk bar tracking"_test = [] {
        auto seq = (C(4) + major_triad) * w
                 | (G(3) + major_triad) * w;
        std::vector<int> bars;
        seq.walk(common, [&](const auto &, auto pos) {
            bars.push_back(pos.bar);
        });
        expect(bars.size() == 2_ul);
        expect(bars[0] == 0_i);
        expect(bars[1] == 1_i);
    };
}
