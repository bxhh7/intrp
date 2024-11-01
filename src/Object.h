#pragma once

#include <any>
#include <cstdint>

class Value
{

};
class Object
{
	Object() {} 

	private:

		uint64_t refcount;
		std::any value;
};
