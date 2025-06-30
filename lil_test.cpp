#include "netvent.hpp"
#include <string>
#include <iostream>
using namespace netvent;

struct Vector2 {
    float x, y;
    Vector2(float x, float y) : x(x), y(y) {}

    Value to_value() const {
        // serialize the vector2
        return Value(map_table({
            {"x", x},
            {"y", y}
        }));
    }
};

// customized player stuct
struct Player {
    int x = 60;
    float y = 60.0f;
    bool visible = true;
    Vector2 velocity = Vector2(0.0f, 0.0f);
    std::string name = "testplayer";

    std::map<std::string, Value> to_serialize() const {
        return {
            // serialize the player
            {"x", Value(x)},
            {"y", Value(y)},
            {"visible", Value(visible)},
            {"velocity", velocity.to_value()},
            {"name", Value(name)}
        };
    }
};

int main() {
    // test the player serialize
    Player player;
    std::string serialized = serialize_to_netvent(Value("new_player"), player.to_serialize());
    std::cout << serialized << std::endl;
}