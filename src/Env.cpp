#include "Env.h"

Env::Env() {push_env();} 

bool Env::lookup(const std::string& key, Object& val)
{
	for (int i = env_vec.size() - 1; i >= 0; i--)
	{
		auto it = env_vec[i].find(key);
		if (it != env_vec[i].end())
		{
			val = it->second;
			return true;
		}
	}
	return false;
}
bool Env::assign(const std::string& key, const Object& val) 
{
	for (int i = env_vec.size() - 1; i >= 0; i--)
	{
		auto it = env_vec[i].find(key);
		if (it != env_vec[i].end())
		{
			it->second = val;
			return true;
		}
	}
	return false;

}
bool Env::lookup(const std::string& key)
{
	Object obj; // yeah well, i dont really care bout performence in this project
 	return this->lookup(key, obj);

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

