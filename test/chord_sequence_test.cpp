#include <boost/ut.hpp>
#include <musicpp/chord_sequence.hpp>
#include <musicpp/scales.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace musicpp::notes;
    using namespace musicpp::intervals;
    using namespace musicpp::durations;
    using namespace musicpp::chord_patterns;
    using namespace musicpp::scale_patterns;
    using namespace std::literals;


    "chord_event from chord * duration"_test = [] {
        auto chord = C(4) + major_triad;
        auto ev = chord * q;
        expect(ev.dur == quarter);
        expect(!ev.is_rest);
        expect(!ev.is_tied);
        expect(ev.chord[0] == C(4));
    };

    "chord_event tied"_test = [] {
        auto ev = ((C(4) + major_triad) * q).tied();
        expect(ev.is_tied);
    };

    "chord_rest construction"_test = [] {
        auto r = chord_rest(h);
        expect(r.is_rest);
        expect(r.dur == half);
    };

    "chord_event str"_test = [] {
        auto ev = (C(4) + major_triad) * q;
        auto s = ev.str();
        expect(s.find("C") != std::string::npos);
        expect(s.find("(q)") != std::string::npos);
    };

    "chord_event rest str"_test = [] {
        auto r = chord_rest(q);
        expect(r.str() == "-(q)"s);
    };

    "chord_event notes_str"_test = [] {
        auto ev = (C(4) + major_triad) * q;
        auto s = ev.notes_str();
        expect(s.find("C4") != std::string::npos);
        expect(s.find("E4") != std::string::npos);
        expect(s.find("G4") != std::string::npos);
    };


    "chord_event analyze"_test = [] {
        auto ev = (C(4) + major_triad) * q;
        auto result = ev.analyze();
        expect(!result.empty());
        expect(result[0].quality == ""s);
    };

    "chord_event analyze with root"_test = [] {
        auto ev = (C(4) + major_triad) * q;
        auto result = ev.analyze(C);
        expect(result.has_value());
        expect(result->root.get_pitch() == C.get_pitch());
    };

    "chord_event analyze in key"_test = [] {
        auto key = C(4) + major;
        auto ev = (C(4) + major_triad) * q;
        auto result = ev.analyze(key);
        expect(!result.empty());
    };


    "two events make sequence"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (G(3) + major_triad) * h;
        expect(seq.length == 2_ul);
    };

    "sequence append event"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (G(3) + major_triad) * h
                 | (F(3) + major_triad) * h;
        expect(seq.length == 3_ul);
    };

    "sequence concatenation"_test = [] {
        auto a = (C(4) + major_triad) * q
               | (G(3) + major_triad) * h;
        auto b = (F(3) + major_triad) * q
               | (D(4) + minor_triad) * h;
        auto c = a | b;
        expect(c.length == 4_ul);
    };


    "sequence total_duration"_test = [] {
        auto seq = (C(4) + major_triad) * h
                 | (G(3) + major_triad) * h;
        expect(seq.total_duration() == whole);
    };

    "sequence total_duration with rest"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | chord_rest(q)
                 | (G(3) + major_triad) * h;
        expect(seq.total_duration() == whole);
    };

    "sequence names"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (A(3) + minor_triad) * q;
        auto n = seq.names();
        expect(n.find("C") != std::string::npos);
        expect(n.find("Am") != std::string::npos);
    };

    "sequence names with rest"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | chord_rest(q);
        auto n = seq.names();
        expect(n.find("C") != std::string::npos);
        expect(n.find("-") != std::string::npos);
    };

    "sequence roman numerals"_test = [] {
        auto key = C(4) + major;
        auto seq = (C(4) + major_triad) * q
                 | (G(3) + major_triad) * q;
        auto r = seq.roman(key);
        expect(r.find("I") != std::string::npos);
        expect(r.find("V") != std::string::npos);
    };


    "sequence str"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (G(3) + major_triad) * h;
        auto s = seq.str();
        expect(!s.empty());
        expect(s.find("(q)") != std::string::npos);
        expect(s.find("(h)") != std::string::npos);
    };

    "sequence notes_str"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (G(3) + major_triad) * h;
        auto s = seq.notes_str();
        expect(s.find("C4") != std::string::npos);
        expect(s.find("G3") != std::string::npos);
    };

    "sequence std::format"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (G(3) + major_triad) * h;
        auto s = std::format("{}", seq);
        expect(!s.empty());
    };


    "sequence for_each visits all"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (G(3) + major_triad) * h
                 | (F(3) + major_triad) * w;
        int count = 0;
        seq.for_each([&](const auto &) { ++count; });
        expect(count == 3_i);
    };


    "slash chord event"_test = [] {
        auto chord = C(4) + major_triad;
        auto slash = chord / G(3);
        auto ev = slash * q;
        expect(ev.chord.size() == 4_ul);
        expect(ev.dur == quarter);
    };


    "mixed voicing sequence"_test = [] {
        auto seq = (C(4) + major_triad) * q
                 | (D(4) + min7) * h;
        expect(seq.length == 2_ul);
        auto d = seq.total_duration();
        expect(d == duration{3, 4});
    };
}
