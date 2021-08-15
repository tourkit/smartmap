#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
	

#include <vector>
#include <set>

// struct Operator;

struct Attribute {

	static inline std::vector<float> attributes;

	unsigned int id; // can be address x.y if attributes is vec of vec

	float cur_val;

	float min_val, max_val, def_val;

	// std::vector<Operator> operators;

	std::set<Attribute*> links_to;

	Attribute* link_from;

	Attribute(float v);

	Attribute(int v);

	Attribute();

	~Attribute();

	void link(Attribute* dst);

	void unlink(Attribute* dst);

	void set(const float& val);

	void update(const float& val);

	const float& get();

	operator float();

	void operator= (const float& val);

	void operator= (const int& val);

};




#endif