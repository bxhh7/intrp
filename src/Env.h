#pragma once

#include <unordered_map>
#include <vector>
#include "Object.h"



class Env
{
	public:
		Env() ;
		using env_map = std::unordered_map<std::string, Object>;

		bool lookup(const std::string&, Object&);
		bool assign(const std::string& s, const Object&);
		bool lookup(const std::string&);
		void bind(const std::string&, const Object&);
		void push_env();
		void pop_env();
	private:
		std::vector<env_map> env_vec;
};

