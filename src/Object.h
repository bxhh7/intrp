#pragma once

#include <any>
#include <string>
#include <variant>
#include <iostream>
/* Temporary Object class to get things working for now. */
enum PrimaryValueType {PRIMARYTYPE_STR, PRIMARYTYPE_INT};
struct PrimaryValue
{
	PrimaryValue(const std::string& v) {type = PRIMARYTYPE_STR; str = v;} ;
	PrimaryValue(int64_t v) {type = PRIMARYTYPE_INT; integer = v;};
	
	PrimaryValueType type;
	std::string str;
	int64_t integer{0};
};

struct Object
{
	Object(int64_t v) : m_primary_value {v} {is_none=false; is_primary = true; m_primary_value.type = PRIMARYTYPE_INT;}
	Object(const std::string& v) : m_primary_value {v} {is_none=false; is_primary = true; m_primary_value.type = PRIMARYTYPE_STR;}
	Object() : m_primary_value{0}, is_none{true} {} 
	bool is_none{1};
	bool is_primary;
	std::string type_name;
	PrimaryValue m_primary_value;
	operator bool() const
	{
		return !is_none;
	}
};

inline std::ostream& operator<<(std::ostream&os , Object& obj)
{
	if (obj.is_none)
	{
		std::cout << "NONE\n";
	}
	else if (!obj.is_primary)
	{
		std::cerr << "TEMPORARY: printing user defined types in currenlty not supported.\n";
	}
	else if (obj.m_primary_value.type == PRIMARYTYPE_INT)
	{
		std::cout << "INT " << obj.m_primary_value.integer << std::endl;
	}
	else if (obj.m_primary_value.type == PRIMARYTYPE_STR)
	{
		std::cout << "STR " << obj.m_primary_value.str << std::endl;
	}
	return os;
}
