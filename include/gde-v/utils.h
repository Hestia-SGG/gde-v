#ifndef _GDE_V_UTILS_H_
#define _GDE_V_UTILS_H_

// Helpers to bind getters and setters for properties
#define GET_NOSET(x, y) y get_## x(){ return this->x; }
#define GET_SET(x, y) GET_NOSET(x, y) void set_## x(y new_val){ this->x = new_val; }

// Get/set but with a sub item
#define GET_SET_STRUCT(x, y, z) y get_## x(){ return this->z.x; } void set_## x(y new_val){ this->z.x = new_val; }

// Binds the getters and setters. Used mostly with GET_SET
#define BIND_GET_SET(x, y, z, ...) ClassDB::bind_method(D_METHOD("get_" #x), &y::get_##x); ClassDB::bind_method(D_METHOD("set_" #x, __VA_ARGS__), &y::set_##x); ADD_PROPERTY(PropertyInfo(Variant::z, #x), "set_" #x, "get_" #x);
#define BIND_GET_NOSET(x, y, z)  ClassDB::bind_method(D_METHOD("get_" #x), &y::get_##x); ADD_PROPERTY(PropertyInfo(Variant::z, #x), "", "get_" #x);
// This one is special as it adds a hint. Should bring it into the one above, but eh.
#define BIND_GET_NOSET_HINT(x, y, z, t, h)  ClassDB::bind_method(D_METHOD("get_" #x), &y::get_##x); ADD_PROPERTY(PropertyInfo(Variant::z, #x, t, h), "", "get_" #x);

// Wraps an enum value into another value.
#define ENUM_WRAP(x, y) x = y##x
#define RV_ENUM_WRAP(x) ENUM_WRAP(x, RV_)
#define SYS_ENUM_WRAP(x) ENUM_WRAP(x, SYS_)

#endif