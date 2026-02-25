#include <format>
#include <iostream>
#include <clocale>

#include <musicpp/chord_sequence.hpp>
#include <musicpp/notes.hpp>
#include <musicpp/timing.hpp>
using namespace musicpp;
using namespace musicpp::notes;
using namespace musicpp::intervals;
using namespace musicpp::durations;
using namespace musicpp::chord_patterns;

int main() {
    std::setlocale(LC_ALL, ".65001");
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    //  春泥棒 (Haru Dorobou / Spring Thief) - ヨルシカ
    //  Chord transcription
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

    auto hd = half.dotted(); // dotted half = 3 beats

    // ── Reusable 2-bar building blocks ──────────────

    // Verse pattern A: | Bbadd9 --- F/C - Dm7 - | --- F/A -- |
    auto vA = (Bb(3)+add9)*hd
            | ((F(3)+major_triad)/C(3))*q
            | (D(4)+min7)*hd
            | ((F(3)+major_triad)/A(2))*q;

    // Verse pattern B: | Bbadd9 --- F/C - Dm7 - | ---- ---- |
    auto vB = (Bb(3)+add9)*hd
            | ((F(3)+major_triad)/C(3))*q
            | (D(4)+min7)*w;

    // 2A variant: Bb6/9 replacing Bbadd9 (pattern A)
    auto v2A = (Bb(3)+maj6_9)*hd
             | ((F(3)+major_triad)/C(3))*q
             | (D(4)+min7)*hd
             | ((F(3)+major_triad)/A(2))*q;

    // 2A variant: BbM9 replacing Bbadd9 (pattern B)
    auto v2B = (Bb(3)+maj9)*hd
             | ((F(3)+major_triad)/C(3))*q
             | (D(4)+min7)*w;

    // Bridge line: | Gm7(11) F/A Bbadd9 -- | N.C. ---- |
    auto bL = (G(3)+min11)*q
            | ((F(3)+major_triad)/A(2))*q
            | (Bb(3)+add9)*h
            | chord_rest(w);

    // Bridge line w/ Dm7: | Gm7(11) F/A Bbadd9 -- | N.C. - Dm7 - |
    auto bLd = (G(3)+min11)*q
             | ((F(3)+major_triad)/A(2))*q
             | (Bb(3)+add9)*h
             | chord_rest(h)
             | (D(4)+min7)*h;

    // Bridge ending: | Bbadd9 --- Csus4 | F ---- |
    auto bE = (Bb(3)+add9)*hd
            | (C(4)+sus4)*q
            | (F(3)+major_triad)*w;

    // Chorus A: | Bb - Csus4 | F/A - Dm7 |
    auto sA = (Bb(3)+major_triad)*h
            | (C(4)+sus4)*h
            | ((F(3)+major_triad)/A(2))*h
            | (D(4)+min7)*h;

    // Chorus A': | Bbadd9 - Csus4 | F/A - Dm7 |
    auto sA2 = (Bb(3)+add9)*h
             | (C(4)+sus4)*h
             | ((F(3)+major_triad)/A(2))*h
             | (D(4)+min7)*h;
    // Chorus B: | Bbadd9 - C7sus4 | Dm7 ---- |
    auto sB = (Bb(3)+add9)*h
            | (C(4)+dom7sus4)*h
            | (D(4)+min7)*w;

    // Chorus C: | Bbadd9 Csus4 F Csus4 | Dm7 ---- |
    auto sC = (Bb(3)+add9)*q
            | (C(4)+sus4)*q
            | (F(3)+major_triad)*q
            | (C(4)+sus4)*q
            | (D(4)+min7)*w;

    // Chorus D: | Bbadd9 - Csus4 | Dm7 ---- |
    auto sD = (Bb(3)+add9)*h
            | (C(4)+sus4)*h
            | (D(4)+min7)*w;

    // Chorus E (3サビ): | Bb - Csus4 | Dm7 ---- |
    auto sE = (Bb(3)+major_triad)*h
            | (C(4)+sus4)*h
            | (D(4)+min7)*w;

    // Chorus ending: | Gm7(11) - Csus4 | F ---- |
    auto sEnd = (G(3)+min11)*h
              | (C(4)+sus4)*h
              | (F(3)+major_triad)*w;

    // Interlude A: | Bbadd9 --- Csus4 | F/A --- Dm7 |
    auto iA = (Bb(3)+add9)*hd
            | (C(4)+sus4)*q
            | ((F(3)+major_triad)/A(2))*hd
            | (D(4)+min7)*q;

    // Interlude B: | Bbadd9 --- C7sus4 | Dm7 ---- |
    auto iB = (Bb(3)+add9)*hd
            | (C(4)+dom7sus4)*q
            | (D(4)+min7)*w;

    // ── Print song structure ────────────────────────
    auto all = vA | vB | vA | vB
             | v2A | v2B | v2A | v2B
             | bL | bLd | bE
             | sA | sB | sC | sB | sA2 | sB | sD | sEnd
             | iA | iB | iA | iB
             | bL | bLd | bE
             | sA2 | sB | sC | sB | sA2 | sB | sD | sEnd;
    auto section = [](const char *name) {
        std::cout << std::format("[{}]\n", name);
    };
    auto line = [](const auto &seq) {
        std::cout << std::format("  {}\n", seq);
    };

    line(all);
}
