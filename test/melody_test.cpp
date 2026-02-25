#include <boost/ut.hpp>
#include <musicpp/melody.hpp>
#include <musicpp/scales.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::notes;
    using namespace musicpp::intervals;
    using namespace musicpp::durations;
    using namespace musicpp::scale_patterns;
    using namespace std::literals;


    "melody_event from note * duration"_test = [] {
        auto ev = C(4) * q;
        expect(ev.pitch == C(4));
        expect(ev.dur == quarter);
        expect(!ev.is_rest);
        expect(!ev.is_tied);
    };

    "rest construction"_test = [] {
        auto r = rest(q);
        expect(r.is_rest);
        expect(r.dur == quarter);
    };

    "melody_event tied"_test = [] {
        auto ev = (C(4) * q).tied();
        expect(ev.is_tied);
        expect(ev.pitch == C(4));
    };


    "two events make melody<2>"_test = [] {
        auto m = C(4) * q | D(4) * q;
        expect(m.size() == 2_ul);
        expect(m[0].pitch == C(4));
        expect(m[1].pitch == D(4));
    };

    "melody append event"_test = [] {
        auto m = C(4) * q | D(4) * q | E(4) * h;
        expect(m.size() == 3_ul);
        expect(m[2].pitch == E(4));
        expect(m[2].dur == half);
    };

    "melody concatenate"_test = [] {
        auto a = C(4) * q | D(4) * q;
        auto b = E(4) * q | F(4) * q;
        auto c = a | b;
        expect(c.size() == 4_ul);
        expect(c[0].pitch == C(4));
        expect(c[2].pitch == E(4));
        expect(c[3].pitch == F(4));
    };


    "melody total_duration"_test = [] {
        auto m = C(4) * q | D(4) * q | E(4) * h;
        expect(m.total_duration() == duration{1, 1});
    };

    "melody note_count excludes rests"_test = [] {
        auto m = C(4) * q | rest(q) | E(4) * q;
        expect(m.note_count() == 2_ul);
    };

    "melody lowest and highest"_test = [] {
        auto m = E(4) * q | C(4) * q | G(4) * q;
        expect(m.lowest() == C(4));
        expect(m.highest() == G(4));
    };

    "melody range"_test = [] {
        auto m = C(4) * q | G(4) * q;
        auto r = m.range();
        expect(r == P5);
    };


    "melody transpose"_test = [] {
        auto m = C(4) * q | E(4) * q;
        auto t = m.transpose(M3);
        expect(t[0].pitch == E(4));
        expect(t[1].pitch.get_pitch() == Gs.get_pitch());
    };

    "melody transpose skips rests"_test = [] {
        auto m = C(4) * q | rest(q) | G(4) * q;
        auto t = m.transpose(P8);
        expect(t[1].is_rest);
        expect(t[0].pitch == C(5));
        expect(t[2].pitch == G(5));
    };

    "melody + interval shorthand"_test = [] {
        auto m = C(4) * q | E(4) * q;
        auto t = m + P8;
        expect(t[0].pitch == C(5));
        expect(t[1].pitch == E(5));
    };

    "melody - interval shorthand"_test = [] {
        auto m = C(5) * q | E(5) * q;
        auto t = m - P8;
        expect(t[0].pitch == C(4));
        expect(t[1].pitch == E(4));
    };

    "melody retrograde"_test = [] {
        auto m = C(4) * q | D(4) * h | E(4) * w;
        auto r = m.retrograde();
        expect(r[0].pitch == E(4));
        expect(r[0].dur == whole);
        expect(r[1].pitch == D(4));
        expect(r[2].pitch == C(4));
    };

    "melody retrograde clears ties"_test = [] {
        auto m = (C(4) * q).tied() | D(4) * q;
        auto r = m.retrograde();
        expect(!r[0].is_tied);
        expect(!r[1].is_tied);
    };

    "melody invert"_test = [] {
        auto m = C(4) * q | E(4) * q | G(4) * q;
        auto inv = m.invert(C(4));
        expect(inv[0].pitch == C(4));
        expect(inv[1].pitch.get_pitch() == Ab.get_pitch());
        expect(inv[2].pitch.get_pitch() == F.get_pitch());
    };

    "melody augment"_test = [] {
        auto m = C(4) * q | D(4) * eighth;
        auto a = m.augment();
        expect(a[0].dur == half);
        expect(a[1].dur == quarter);
    };

    "melody diminish"_test = [] {
        auto m = C(4) * h | D(4) * q;
        auto d = m.diminish();
        expect(d[0].dur == quarter);
        expect(d[1].dur == eighth);
    };


    "melody repeat"_test = [] {
        auto m = C(4) * q | E(4) * q;
        auto r = m.repeat<3>();
        expect(r.size() == 6_ul);
        expect(r[0].pitch == C(4));
        expect(r[2].pitch == C(4));
        expect(r[4].pitch == C(4));
    };


    "melody is_diatonic true"_test = [] {
        auto key = C(4) + major;
        auto m = C(4) * q | D(4) * q | E(4) * q;
        expect(m.is_diatonic(key));
    };

    "melody is_diatonic false"_test = [] {
        auto key = C(4) + major;
        auto m = C(4) * q | Fs(4) * q;
        expect(!m.is_diatonic(key));
    };

    "melody is_diatonic ignores rests"_test = [] {
        auto key = C(4) + major;
        auto m = C(4) * q | rest(q) | E(4) * q;
        expect(m.is_diatonic(key));
    };


    "melody_event str note"_test = [] {
        auto ev = C(4) * q;
        expect(ev.str() == "C4(q)"s);
    };

    "melody_event str rest"_test = [] {
        auto r = rest(h);
        expect(r.str() == "-(h)"s);
    };

    "melody_event str tied"_test = [] {
        auto ev = (D(4) * q).tied();
        expect(ev.str() == "D4(q)~"s);
    };

    "melody str"_test = [] {
        auto m = C(4) * q | rest(eighth) | E(4) * h;
        auto s = m.str();
        expect(s == "C4(q) -(8th) E4(h)"s);
    };

    "melody std::format"_test = [] {
        auto m = C(4) * q | D(4) * q;
        auto s = std::format("{}", m);
        expect(s == "C4(q) D4(q)"s);
    };
}
