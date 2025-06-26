#include "netvent.hpp"
#include <cassert>
#include <iostream>

using namespace netvent;

void test_simple_array() {
    std::vector<Value> vec = {1, 2, 3};
    Table t(vec);
    std::string serialized = t.serialize();
    assert(serialized == "[1,2,3]");
    Table deserialized = Table::deserialize(serialized);
    assert(deserialized.get_is_array());
}

void test_nested_structure() {
    // Create a table with nested array
    Table inner_array(std::vector<Value>{1, 2, 3});
    std::map<Value, Value> map;
    map[Value("array")] = Value("test");
    map[Value("nested")] = Value(1.5f);
    Table t(map);
    std::string serialized = t.serialize();
    Table deserialized = Table::deserialize(serialized);
    assert(!deserialized.get_is_array());
}

void test_empty_structures() {
    Table empty_array(std::vector<Value>{});
    assert(empty_array.serialize() == "[]");
    
    Table empty_table;
    assert(empty_table.serialize() == "{}");
    
    Table parsed_empty_array = Table::deserialize("[]");
    assert(parsed_empty_array.get_is_array());
    
    Table parsed_empty_table = Table::deserialize("{}");
    assert(!parsed_empty_table.get_is_array());
}

void test_table_value() {
    // Create a table with a nested table as a value
    Table inner(std::vector<Value>{1, 2, 3});
    
    std::map<Value, Value> outer_map;
    outer_map[Value("nested")] = Value(inner);
    outer_map[Value("simple")] = Value(42);
    
    Table outer(outer_map);
    std::string serialized = outer.serialize();
    Table deserialized = Table::deserialize(serialized);
    
    // Verify the structure
    assert(!deserialized.get_is_array());
    Value& nested = deserialized[Value("nested")];
    assert(nested.is_table());
    assert(nested.as_table().get_is_array());
}

void test_netvent_format() {
    std::string input = R"(// comments can also be like this
"shoot" // event name
x 0 // int
y 0.1 // float
player_name "this person" // string
gun_active true // bool)";

    auto [event, data] = deserialize_from_netvent(input);
    
    assert(event.is_string() && event.as_string() == "shoot");
    assert(data["x"].is_int() && data["x"].as_int() == 0);
    assert(data["y"].is_float() && data["y"].as_float() == 0.1f);
    assert(data["player_name"].is_string() && data["player_name"].as_string() == "this person");
    assert(data["gun_active"].is_bool() && data["gun_active"].as_bool() == true);

    // Test serialization
    std::map<std::string, Value> data_to_serialize;
    data_to_serialize["x"] = Value(0);
    data_to_serialize["y"] = Value(0.1f);
    data_to_serialize["player_name"] = Value("this person");
    data_to_serialize["gun_active"] = Value(true);

    std::string serialized = serialize_to_netvent(Value("shoot"), data_to_serialize);
    
    // Verify we can deserialize what we just serialized
    auto [event2, data2] = deserialize_from_netvent(serialized);
    
    assert(event2.is_string() && event2.as_string() == "shoot");
    assert(data2["x"].is_int() && data2["x"].as_int() == 0);
    assert(data2["y"].is_float() && data2["y"].as_float() == 0.1f);
    assert(data2["player_name"].is_string() && data2["player_name"].as_string() == "this person");
    assert(data2["gun_active"].is_bool() && data2["gun_active"].as_bool() == true);

    // Verify the exact format matches what we expect
    std::string expected = R"("shoot"
gun_active true
player_name "this person"
x 0
y 0.1
)";
    assert(serialized == expected);
}

void test_value_parsing() {
    // Test number parsing
    assert(Value::deserialize("42").is_int());
    assert(Value::deserialize("42").as_int() == 42);
    assert(Value::deserialize("42.0").is_float());
    assert(Value::deserialize("42.0").as_float() == 42.0f);
    assert(Value::deserialize("-42").is_int());
    assert(Value::deserialize("-42").as_int() == -42);
    assert(Value::deserialize("-42.5").is_float());
    assert(Value::deserialize("-42.5").as_float() == -42.5f);

    // Test boolean parsing
    assert(Value::deserialize("true").is_bool());
    assert(Value::deserialize("true").as_bool() == true);
    assert(Value::deserialize("false").is_bool());
    assert(Value::deserialize("false").as_bool() == false);

    // Test string parsing
    assert(Value::deserialize("\"hello\"").is_string());
    assert(Value::deserialize("\"hello\"").as_string() == "hello");
    
    // Test table parsing
    std::vector<Value> vec{1, 2, 3};
    Table t(vec);
    std::string table_str = t.serialize();
    Value v = Value::deserialize(table_str);
    assert(v.is_table());
    assert(v.as_table().get_is_array());
}

void test_array_of_objects() {
    std::vector<Value> rectangles;
    
    // first rectangle
    std::map<Value, Value> rect1;
    rect1[Value("x")] = Value(10);
    rect1[Value("y")] = Value(20);
    rect1[Value("width")] = Value(100);
    rect1[Value("height")] = Value(50);
    rectangles.push_back(Value(Table(rect1)));

    // second rectangle
    std::map<Value, Value> rect2;
    rect2[Value("x")] = Value(30);
    rect2[Value("y")] = Value(40);
    rect2[Value("width")] = Value(200);
    rect2[Value("height")] = Value(75);
    rectangles.push_back(Value(Table(rect2)));

    // create array table
    Table array_table(rectangles);
    std::string serialized = array_table.serialize();
    
    // expected 
    std::string expected = R"([{"height"=50,"width"=100,"x"=10,"y"=20},{"height"=75,"width"=200,"x"=30,"y"=40}])";
    assert(serialized == expected);

    // test deserialization
    Table deserialized = Table::deserialize(serialized);
    assert(deserialized.get_is_array());
    
    auto data = std::get<std::vector<Value>>(deserialized.get_data());
    assert(data.size() == 2);
    
    // check first rectangle
    assert(data[0].is_table());
    Table& first_rect = data[0].as_table();
    assert(first_rect[Value("x")].as_int() == 10);
    assert(first_rect[Value("y")].as_int() == 20);
    assert(first_rect[Value("width")].as_int() == 100);
    assert(first_rect[Value("height")].as_int() == 50);

    // check second rectangle
    assert(data[1].is_table());
    Table& second_rect = data[1].as_table();
    assert(second_rect[Value("x")].as_int() == 30);
    assert(second_rect[Value("y")].as_int() == 40);
    assert(second_rect[Value("width")].as_int() == 200);
    assert(second_rect[Value("height")].as_int() == 75);
}

// warning: stretches parser to its limits kinda
void test_deeply_nested() {
    // create innermost empty table
    Table empty_table;
    empty_table[Value("eyes_bleeding")] = Value(true);
    
    // wrap it in an array
    std::vector<Value> inner_array;
    inner_array.push_back(Value(empty_table));
    inner_array.push_back(Value(empty_table));  // add it twice
    Table array_with_empty = Table(inner_array);
    
    // create middle table containing that array
    std::map<Value, Value> middle_map;
    middle_map[Value("nested")] = Value(array_with_empty);
    middle_map[Value("data")] = Value(42);
    Table middle_table(middle_map);
    
    // put that in another array
    std::vector<Value> outer_array;
    outer_array.push_back(Value(middle_table));
    outer_array.push_back(Value(middle_table));  // add it twice
    Table final_table(outer_array);
    
    // should look like: [{nested=[{eyes_bleeding=true},{eyes_bleeding=true}],data=42},{nested=[{eyes_bleeding=true},{eyes_bleeding=true}],data=42}] (my eyes are bleeding)
    std::string serialized = final_table.serialize();
    std::string expected = R"([{"data"=42,"nested"=[{"eyes_bleeding"=true},{"eyes_bleeding"=true}]},{"data"=42,"nested"=[{"eyes_bleeding"=true},{"eyes_bleeding"=true}]}])";
    assert(serialized == expected);
    
    // test deserialization and verify structure
    Table deserialized = Table::deserialize(serialized);
    assert(deserialized.get_is_array());
    
    auto outer_data = std::get<std::vector<Value>>(deserialized.get_data());
    assert(outer_data.size() == 2);  // now two elements
    
    for (Value& outer_elem : outer_data) {
        assert(outer_elem.is_table());
        Table& middle = outer_elem.as_table();
        assert(!middle.get_is_array());
        assert(middle[Value("data")].is_int());
        assert(middle[Value("data")].as_int() == 42);
        assert(middle[Value("nested")].is_table());
        
        Table& inner_array_table = middle[Value("nested")].as_table();
        assert(inner_array_table.get_is_array());
        
        auto inner_data = std::get<std::vector<Value>>(inner_array_table.get_data());
        assert(inner_data.size() == 2);  // now two elements
        
        for (Value& inner_elem : inner_data) {
            assert(inner_elem.is_table());
            Table& innermost = inner_elem.as_table();
            assert(!innermost.get_is_array());
            assert(innermost[Value("eyes_bleeding")].is_bool());
            assert(innermost[Value("eyes_bleeding")].as_bool() == true);
        }
    }
}

void test_trailing_commas() {
    // test array with trailing comma
    std::string array_with_comma = "[1, 2, 3,]";
    Table array = Table::deserialize(array_with_comma);
    assert(array.get_is_array());
    auto array_data = std::get<std::vector<Value>>(array.get_data());
    assert(array_data.size() == 3);
    assert(array_data[0].as_int() == 1);
    assert(array_data[1].as_int() == 2);
    assert(array_data[2].as_int() == 3);
    
    // test object with trailing comma
    std::string object_with_comma = R"({"x"=10,"y"=20,})"; 
    Table object = Table::deserialize(object_with_comma);
    if (object[Value("y")].is_string()) {
        std::cout << "y as string: '" << object[Value("y")].as_string() << "'" << std::endl;
    }
    
    assert(!object.get_is_array());
    assert(object[Value("x")].as_int() == 10);
    assert(object[Value("y")].as_int() == 20);
    
    // test nested structure with trailing commas
    std::string nested = R"([{"a"=1,},{"b"=2,},])"; 
    Table nested_table = Table::deserialize(nested);
    assert(nested_table.get_is_array());
    auto nested_data = std::get<std::vector<Value>>(nested_table.get_data());
    assert(nested_data.size() == 2);
    assert(nested_data[0].as_table()[Value("a")].as_int() == 1);
    assert(nested_data[1].as_table()[Value("b")].as_int() == 2);
}

int main() {
    test_simple_array();
    test_nested_structure();
    test_empty_structures();
    test_table_value();
    test_netvent_format();
    test_value_parsing();
    test_array_of_objects();
    test_deeply_nested();
    test_trailing_commas();
    std::cout << "All tests passed!" << std::endl;
    return 0;
} 