#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
	
#include <vector>
#include <set>
#include <memory>

struct Operator {};

struct Attribute {

	static inline std::set<Attribute*> pool;
	

	static inline std::vector<float> data;


	unsigned int id = 0; // can be address x.y if attributes is vec of vec

	float cur_val = 0;

	float min_val, max_val, def_val;


	std::set<Attribute*> links;

	std::set<Operator> operators;
	

	Attribute(float* target);

	Attribute(float v);

	Attribute(int v);

	Attribute();

	~Attribute();

	void link(Attribute* dst);

	void set(const float& val);

	virtual void update(const float& val);

	const float& get();

	operator float();

	void operator= (const float& val);

	void operator= (const int& val);

};

// struct DMXAttribute {

// 	enum Mode { NORMAL, FINE, ULTRA };

// 	DMXAttribute(unsigned char* target, Mode NORMAL) {

// 		set()
// 	}


// };

#endif