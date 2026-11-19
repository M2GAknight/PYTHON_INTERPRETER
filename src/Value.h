#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
struct Value
{
    enum Tag
    {
        NONE,
        BOOL,
        INT,
        FLOAT,
        STR,
        TUPLE
    } tag = NONE;
    long long i = 0;          // tag == INT
    double f = 0;             // tag == FLOAT
    bool b = false;           // tag == BOOL
    std::string s;            // tag == STR
    std::vector<Value> items; // tag == TUPLE
    static Value boolean(bool v)
    {
        Value r;
        r.tag = BOOL;
        r.b = v;
        return r;
    }
    static Value num(long long v)
    {
        Value r;
        r.tag = INT;
        r.i = v;
        return r;
    }
    static Value flt(double v)
    {
        Value r;
        r.tag = FLOAT;
        r.f = v;
        return r;
    }
    static Value str(std::string v)
    {
        Value r;
        r.tag = STR;
        r.s = std::move(v);
        return r;
    }
    static Value tuple(std::vector<Value> x)
    {
        Value r;
        r.tag = TUPLE;
        r.items = x;
        return r;
    }
};
inline Value operator+(const Value &a, const Value &b)
{
    Value ans;
    if (a.tag == Value::STR && b.tag == Value::STR)
    {
        ans.tag = Value::STR;
        ans.s = a.s + b.s;
        return ans;
    }
    else if (a.tag == Value::BOOL && b.tag == Value::BOOL)
    {
        ans.tag = Value::INT;
        ans.i = a.b + b.b;
        return ans;
    }
    else if (a.tag == Value::BOOL && b.tag == Value::INT)
    {
        ans.tag = Value::INT;
        ans.i = a.b + b.i;
        return ans;
    }
    else if (a.tag == Value::INT && b.tag == Value::BOOL)
    {
        ans.tag = Value::INT;
        ans.i = a.i + b.b;
        return ans;
    }
    else if (a.tag == Value::INT && b.tag == Value::INT)
    {
        ans.tag = Value::INT;
        ans.i = a.i + b.i;
        return ans;
    }
    else if (a.tag == Value::FLOAT && b.tag == Value::FLOAT)
    {
        ans.tag = Value::FLOAT;
        ans.f = a.f + b.f;
        return ans;
    }
    else if (a.tag == Value::FLOAT && b.tag == Value::INT)
    {
        ans.tag = Value::FLOAT;
        ans.f = a.f + b.i;
        return ans;
    }
    else if (a.tag == Value::INT && b.tag == Value::FLOAT)
    {
        ans.tag = Value::FLOAT;
        ans.f = a.i + b.f;
        return ans;
    }
    else if (a.tag == Value::FLOAT && b.tag == Value::BOOL)
    {
        ans.tag = Value::FLOAT;
        ans.f = a.f + b.b;
        return ans;
    }
    else if (a.tag == Value::BOOL && b.tag == Value::FLOAT)
    {
        ans.tag = Value::FLOAT;
        ans.f = a.b + b.f;
        return ans;
    }
    else
    {
        std::cout << "wanyuanshenwande";
        exit(0);
        return Value{};
    }
}
inline Value operator-(const Value &a, const Value &b)
{
    if (a.tag == Value::BOOL && b.tag == Value::BOOL)
        return Value::num(a.b - b.b);
    if (a.tag == Value::BOOL && b.tag == Value::INT)
        return Value::num(a.b - b.i);
    if (a.tag == Value::INT && b.tag == Value::BOOL)
        return Value::num(a.i - b.b);
    if (a.tag == Value::INT && b.tag == Value::INT)
        return Value::num(a.i - b.i);
    if (a.tag == Value::BOOL && b.tag == Value::FLOAT)
        return Value::flt(a.b - b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::BOOL)
        return Value::flt(a.f - b.b);
    if (a.tag == Value::INT && b.tag == Value::FLOAT)
        return Value::flt(a.i - b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::INT)
        return Value::flt(a.f - b.i);
    if (a.tag == Value::FLOAT && b.tag == Value::FLOAT)
        return Value::flt(a.f - b.f);
    std::cout << "wanyuanshenwande";
    exit(0);
    return Value{};
}

inline Value operator*(const Value &a, const Value &b)
{
    if (a.tag == Value::STR && (b.tag == Value::INT || b.tag == Value::BOOL))
    {
        long long n = (b.tag == Value::BOOL) ? (b.b ? 1 : 0) : b.i;
        std::string s;
        for (long long k = 0; k < n; ++k)
            s += a.s;
        return Value::str(s);
    }
    if (b.tag == Value::STR && (a.tag == Value::INT || a.tag == Value::BOOL))
    {
        long long n = (a.tag == Value::BOOL) ? (a.b ? 1 : 0) : a.i;
        std::string s;
        for (long long k = 0; k < n; ++k)
            s += b.s;
        return Value::str(s);
    }
    if (a.tag == Value::BOOL && b.tag == Value::BOOL)
        return Value::num(a.b * b.b);
    if (a.tag == Value::BOOL && b.tag == Value::INT)
        return Value::num(a.b * b.i);
    if (a.tag == Value::INT && b.tag == Value::BOOL)
        return Value::num(a.i * b.b);
    if (a.tag == Value::INT && b.tag == Value::INT)
        return Value::num(a.i * b.i);
    if (a.tag == Value::BOOL && b.tag == Value::FLOAT)
        return Value::flt(a.b * b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::BOOL)
        return Value::flt(a.f * b.b);
    if (a.tag == Value::INT && b.tag == Value::FLOAT)
        return Value::flt(a.i * b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::INT)
        return Value::flt(a.f * b.i);
    if (a.tag == Value::FLOAT && b.tag == Value::FLOAT)
        return Value::flt(a.f * b.f);
    std::cout << "wanyuanshenwande";
    exit(0);
    return Value{};
}

inline Value operator/(const Value &a, const Value &b)
{
    if (a.tag == Value::BOOL && b.tag == Value::BOOL)
        return Value::flt((double)a.b / b.b);
    if (a.tag == Value::BOOL && b.tag == Value::INT)
        return Value::flt(a.b / (double)b.i);
    if (a.tag == Value::INT && b.tag == Value::BOOL)
        return Value::flt(a.i / (double)b.b);
    if (a.tag == Value::INT && b.tag == Value::INT)
        return Value::flt((double)a.i / b.i);
    if (a.tag == Value::BOOL && b.tag == Value::FLOAT)
        return Value::flt(a.b / b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::BOOL)
        return Value::flt(a.f / b.b);
    if (a.tag == Value::INT && b.tag == Value::FLOAT)
        return Value::flt(a.i / b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::INT)
        return Value::flt(a.f / b.i);
    if (a.tag == Value::FLOAT && b.tag == Value::FLOAT)
        return Value::flt(a.f / b.f);
    std::cout << "wanyuanshenwande";
    exit(0);
    return Value{};
}
inline long long floor_div_ll(long long x, long long y)
{
    long long q = x / y;
    if (x % y != 0 && ((x < 0) != (y < 0)))
        --q;
    return q;
}

inline long long floor_mod_ll(long long x, long long y)
{
    long long r = x % y;
    if (r != 0 && ((r < 0) != (y < 0)))
        r += y;
    return r;
}

inline Value floordiv(const Value &a, const Value &b)
{
    if (a.tag == Value::BOOL && b.tag == Value::BOOL)
        return Value::num(floor_div_ll(a.b, b.b));
    if (a.tag == Value::BOOL && b.tag == Value::INT)
        return Value::num(floor_div_ll(a.b, b.i));
    if (a.tag == Value::INT && b.tag == Value::BOOL)
        return Value::num(floor_div_ll(a.i, b.b));
    if (a.tag == Value::INT && b.tag == Value::INT)
        return Value::num(floor_div_ll(a.i, b.i));
    if (a.tag == Value::BOOL && b.tag == Value::FLOAT)
        return Value::flt(std::floor(a.b / b.f));
    if (a.tag == Value::FLOAT && b.tag == Value::BOOL)
        return Value::flt(std::floor(a.f / b.b));
    if (a.tag == Value::INT && b.tag == Value::FLOAT)
        return Value::flt(std::floor(a.i / b.f));
    if (a.tag == Value::FLOAT && b.tag == Value::INT)
        return Value::flt(std::floor(a.f / b.i));
    if (a.tag == Value::FLOAT && b.tag == Value::FLOAT)
        return Value::flt(std::floor(a.f / b.f));
    std::cout << "wanyuanshenwande";
    exit(0);
    return Value{};
}

inline Value operator%(const Value &a, const Value &b)
{
    if (a.tag == Value::BOOL && b.tag == Value::BOOL)
        return Value::num(floor_mod_ll(a.b, b.b));
    if (a.tag == Value::BOOL && b.tag == Value::INT)
        return Value::num(floor_mod_ll(a.b, b.i));
    if (a.tag == Value::INT && b.tag == Value::BOOL)
        return Value::num(floor_mod_ll(a.i, b.b));
    if (a.tag == Value::INT && b.tag == Value::INT)
        return Value::num(floor_mod_ll(a.i, b.i));
    if (a.tag == Value::BOOL && b.tag == Value::FLOAT)
        return Value::flt(a.b - std::floor(a.b / b.f) * b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::BOOL)
        return Value::flt(a.f - std::floor(a.f / b.b) * b.b);
    if (a.tag == Value::INT && b.tag == Value::FLOAT)
        return Value::flt(a.i - std::floor(a.i / b.f) * b.f);
    if (a.tag == Value::FLOAT && b.tag == Value::INT)
        return Value::flt(a.f - std::floor(a.f / b.i) * b.i);
    if (a.tag == Value::FLOAT && b.tag == Value::FLOAT)
        return Value::flt(a.f - std::floor(a.f / b.f) * b.f);
    std::cout << "wanyuanshenwande";
    exit(0);
    return Value{};
}
inline Value muldivmod(const std::string &op, const Value &a, const Value &b)
{
	if (op == "*")
		return a * b;
	if (op == "/")
		return a / b;
	if (op == "//")
		return floordiv(a, b);
	if (op == "%")
		return a % b;
	std::cout << "wanyuanshenwande";
	exit(0);
	return Value{};
}