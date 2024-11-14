#pragma once

#include <unordered_map>
#include <vector>
#include "Object.h"



class Env
{
	public:
		Env() ;
		using env_map = std::unordered_map<std::string, Object>;

		Object lookup(const std::string&);
		void bind(const std::string&, const Object&);
		void push_env();
		void pop_env();
	private:
		std::vector<env_map> env_vec;
};

