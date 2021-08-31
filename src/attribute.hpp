#ifndef Number_H
#define Number_H
	
#include <vector>
#include <set>
#include <memory>

struct Operator {};

struct Attribute {

	static inline std::set<Attribute*> pool;

	std::set<Attribute*> links;

	Attribute() { pool.insert(this); }

	virtual void update() {}

};

struct Number : public Attribute {


	float cur_val = 0;

	float min_val, max_val, def_val;

	std::set<Operator> operators;

<<<<<<< Updated upstream
	Attribute(float v);
=======

	Number(float v);
>>>>>>> Stashed changes

	Number(int v);

	Number();

	~Number();

	void link(Number* dst);

	void set(const float& val);

	void update(const float& val);

	const float& get();

	operator float();

	void operator= (const float& val);

	void operator= (const int& val);

};

<<<<<<< Updated upstream

// struct DMXAttribute {
=======
struct ShaderNumber : public Number {

	static inline std::vector<float> data;

	unsigned int id = 0; // can be address x.y if Numbers is vec of vec

	ShaderNumber(float v);

	ShaderNumber(int v);

	ShaderNumber();

	~ShaderNumber();


};

// struct DMXNumber {
>>>>>>> Stashed changes

// 	enum Mode { NORMAL, FINE, ULTRA };

// 	DMXNumber(unsigned char* target, Mode NORMAL) {

// 		set()
// 	}


// };

#endif