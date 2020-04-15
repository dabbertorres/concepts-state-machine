#pragma once

#include <tuple>
#include <variant>

#include "concept_utils.hpp"

namespace freeze
{

// If a state satisifies this concept, it is a non-terminal state, and can transition to other states.
// Any state that does not satisify this concept is a terminal state, and is an end condition for the machine.
template<typename T, typename M>
concept Transitionable = requires(const T& t, M& m)
{
    t.transition(m);
};

// if a state satisifies this concept, it's reset function will be called when it is transitioned to.
template<typename T>
concept Resettable = requires(T& t)
{
    t.reset();
};

template<Callable... States>
class machine
{
public:
    machine() = default;

    // clang-format off
    machine(States&&... states) :
        states{std::make_tuple(states...)},
        current{&std::get<0>(this->states)}
    {}
    // clang-format on

    machine(const machine&) = default;
    machine& operator=(const machine&) = default;

    machine(machine&&) = default;
    machine& operator=(machine&&) = default;

    ~machine() = default;

    template<typename... Inputs>
    void update(Inputs&&... inputs)
    {
        auto visitor = [&inputs...](auto* state) {
            // invoke only if these inputs are relevant for the current state.
            if constexpr (Callable<decltype(*state), Inputs...>) state->operator()(std::forward<Inputs>(inputs)...);
        };
        std::visit(visitor, current);

        transition();
    }

    template<Callable S>
    void transition_to() noexcept
    {
        current = &std::get<S>(states);

        // reset new state if it has that feature
        auto visitor = [](auto* state) {
            if constexpr (Resettable<decltype(*state)>) state->reset();
        };
        std::visit(visitor, current);
    }

    template<Callable State>
    const State& get_state() const noexcept
    {
        return std::get<State>(states);
    }

    // current_state returns the index of the current state.
    size_t current_state() const noexcept { return current.index(); }

    // reset re-initializes every state and resets to the initial state.
    void reset()
    {
        states  = std::make_tuple<States...>();
        current = &std::get<0>(states);
    }

private:
    void transition()
    {
        auto visitor = [this](const auto* state) {
            // we can only transition from non-terminal states
            if constexpr (Transitionable<decltype(*state), decltype(*this)>) state->transition(*this);
        };
        std::visit(visitor, current);
    }

    std::tuple<States...>    states;
    std::variant<States*...> current = &std::get<0>(states);
};

}
