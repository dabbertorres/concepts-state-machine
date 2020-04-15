#include <iostream>

#include "state_machine.hpp"

struct Y;

struct X
{
    int foo = 0;

    void operator()() { foo++; }
    void operator()(int v) { foo += v; }

    template<typename Machine>
    void transition(Machine& machine) const
    {
        if (foo > 3) machine.template transition_to<Y>();
    }

    void reset() { foo = 0; }
};

struct Y
{
    float bar = 0.f;

    void operator()() { bar += 0.5f; }
    // void operator()(float v) { bar += v; }

    template<typename Machine>
    void transition(Machine& machine) const
    {
        if (bar > 1.5f) machine.template transition_to<X>();
    }
};

using namespace std::string_literals;

int main(int /*argc*/, char** /*argv*/)
{
    std::cout << sizeof(std::variant<int*, double*, X*, Y*>) << std::endl;
    std::cout << std::boolalpha;

    freeze::machine machine(X{}, Y{});

    std::cout << "sizeof(machine<X, Y>) = " << sizeof(machine) << std::endl;

    //                         <state> = <value after update>
    machine.update();        // X      = 1
    machine.update();        // X      = 2
    machine.update();        // X      = 3
    machine.update("hello"); // X      = 3   (no change)
    machine.update();        // X      = 4
    machine.update();        // Y      = 0.5
    machine.update();        // Y      = 1.0
    machine.update(2.75);    // Y      = 1.0 (no change)
    machine.update(5);       // Y      = 1.0 (no change)
    machine.update();        // Y      = 1.5
    machine.update();        // Y      = 2.0

    const auto& x = machine.get_state<X>();
    const auto& y = machine.get_state<Y>();

    std::cout << "x.foo = " << x.foo << std::endl; // 1
    std::cout << "y.bar = " << y.bar << std::endl; // 4.25

    return 0;
}
