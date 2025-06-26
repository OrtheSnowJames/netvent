# Netvent

A little **serializer/deserializer** that has custom **table and value classes** for storing and getting data, a **simple but working syntax** to transmit data easier over the network and **built-in c++ types** (last one should not be a flex)

## Naming

Named netvent by smashing **net and event together**. Any other recommedations for names are highly appreciated!

## Getting It Up

If you really want to, you can clone the whole repository:

```sh
git clone https://github.com/OrtheSnowJames/netvent
```

Or you could just clone the netvent.hpp file (has everything):

```sh
svn export https://github.com/OrtheSnowJames/netvent/trunk/netvent.hpp
```

Then put the file in your repo source folder, include it and you're good to go!

## Docs

Some people (like me) like examples better than docs. The test.cpp file has examples of 9/10 (ratio) of the edge cases.

If you want docs (also me) here are some below:

### Value Class

The `Value` class is a variant type that can hold different data types:
- `int`: Int numbers
- `float`: Float numbers
- `bool`: Bool values (true/false)
- `string`: Strings
- `Table`: Nested table (array/object) structures

Methods:
```cpp
Value();                     // Creates a null value (0)
Value(int v);                // Create from integer
Value(float v);              // Create from float
Value(bool v);               // Create from boolean
Value(const char* v);        // Create from string literal
Value(const std::string& v); // Create from string
Value(const Table& v);       // Create from table

// Type checking
bool is_int() const;       // Check if value is integer
bool is_float() const;     // Check if value is float
bool is_bool() const;      // Check if value is boolean
bool is_string() const;    // Check if value is string
bool is_table() const;     // Check if value is table

// Value getters
int as_int() const;             // Get as integer
float as_float() const;         // Get as float
bool as_bool() const;           // Get as boolean
std::string as_string() const;  // Get as string
const Table& as_table() const;  // Get as table reference
Table& as_table();              // Get as mutable table reference

// Serialization
std::string serialize() const;  // Convert to string format
static Value deserialize(const std::string& data); // Parse from string
```

### Table Class

The `Table` class can represent either a map or an array:
- Map mode: Stores key-value pairs where both key and value are `Value` objects
- Array mode: Stores indexed values (automatically uses integers as keys)

Methods:
```cpp
Table();                                 // Create empty table (map mode)
Table(const std::map<Value, Value>& d);  // Create from map
Table(const std::vector<Value>& d);      // Create from vector (array mode)

Value& operator[](const Value& key);     // Access/modify values
bool get_is_array() const;               // Check if table is in array mode

// Get Internal Data 
std::variant<std::map<Value, Value>, std::vector<Value>> get_data() const;

// Serialization
std::string serialize() const;        // Convert to string format
static Table deserialize(const std::string& data);  // Parse from string
```

### Serialization Functions

High-level functions for event-based serialization:

```cpp
// Serialize an event with data
std::string serialize_to_netvent(
    const Value& event_name,
    const std::map<std::string, Value>& data
);

// Deserialize an event and its data
std::pair<Value, std::map<std::string, Value>> 
    deserialize_from_netvent(std::string data);
```

### Format Examples

1. Simple event with data:
```
"shoot"
x 0
y 0.1
player_name "this person"
gun_active true
```

2. Array of objects:
```
[{"height"=50,"width"=100,"x"=10,"y"=20},{"height"=75,"width"=200,"x"=30,"y"=40}]
```

3. Nested structures:
```
{
    "array"="test",
    "nested"=1.5
}
```

### Format Rules

1. Comments:
```
// This is a comment
event_name // Inline comment
key value // Another comment
```
The format supports C-style line comments. It does NOT support multiline comments.

2. Trailing Commas:
Trailing commas are allowed in tables for easier editing and version control:
```
[1, 2, 3,]           // Valid: trailing comma in array
{"x"=1, "y"=2,}      // Valid: trailing comma in object
[{"a"=1,}, {"b"=2,},]  // Valid: multiple trailing commas
```

This matches common JSON-like formats where trailing commas are allowed to make diffs cleaner when adding new items.
