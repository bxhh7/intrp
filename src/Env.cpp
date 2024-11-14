#include "Env.h"

Env::Env() {push_env();} 

Object Env::lookup(const std::string& key)
{
	for (int i = env_vec.size() - 1; i >= 0; i--)
	{
		auto it = env_vec[i].find(key);
		if (it != env_vec[i].end())
		{
			return it->second;
		}
	}
	return Object{};
}

void Env::bind(const std::string& key, const Object& val)
{
	/* bind to the top-of-the-stack env */
	env_vec[env_vec.size()-1][key] = val;
}
void Env::push_env()
{
	env_vec.push_back(env_map{});
}
void Env::pop_env()
{
	env_vec.pop_back();
}

