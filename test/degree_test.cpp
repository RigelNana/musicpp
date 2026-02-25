#include <boost/ut.hpp>
#include <musicpp/degree.hpp>

int main() {
    using namespace boost::ut;
    using namespace musicpp;
    using namespace std::literals;


    "degree default"_test = [] {
        degree d;
        expect(d.num == 0_i);
        expect(d.alter == 0_i);
    };

    "degree from int"_test = [] {
        degree d{5};
        expect(d.num == 5_i);
        expect(d.alter == 0_i);
    };

    "degree with alteration"_test = [] {
        degree d{6, -1};
        expect(d.num == 6_i);
        expect(d.alter == -1_i);
    };


    "b() creates flat degree"_test = [] {
        constexpr auto d = b(7);
        expect(d.num == 7_i);
        expect(d.alter == -1_i);
    };

    "s() creates sharp degree"_test = [] {
        constexpr auto d = s(4);
        expect(d.num == 4_i);
        expect(d.alter == 1_i);
    };


    "degree bool: valid"_test = [] {
        expect(static_cast<bool>(degree{1}));
        expect(static_cast<bool>(degree{7}));
    };

    "degree bool: invalid (sentinel)"_test = [] {
        expect(!static_cast<bool>(degree{}));
        expect(!static_cast<bool>(degree{0}));
    };


    "degree equality"_test = [] {
        expect(degree{1} == degree{1});
        expect(degree{1} != degree{2});
        expect(degree{6, -1} == degree{6, -1});
        expect(degree{6, -1} != degree{6, 0});
        expect(degree{6, -1} != degree{7, -1});
    };


    "degree prefix natural"_test = [] {
        expect(degree{1}.prefix() == ""s);
    };

    "degree prefix flat"_test = [] {
        expect(degree{7, -1}.prefix() == "b"s);
        expect(b(6).prefix() == "b"s);
    };

    "degree prefix sharp"_test = [] {
        expect(degree{4, +1}.prefix() == "#"s);
        expect(s(4).prefix() == "#"s);
    };


    "b and s are consteval"_test = [] {
        constexpr degree flat7 = b(7);
        constexpr degree sharp4 = s(4);
        static_assert(flat7.num == 7);
        static_assert(flat7.alter == -1);
        static_assert(sharp4.num == 4);
        static_assert(sharp4.alter == 1);
        expect(true);
    };
}
