#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>


#include "attribute.hpp"
#include "operator.hpp"
#include "globals.hpp"


int main() {



Attribute* a1 = new Attribute;
Attribute* a2 = new Attribute; 
Attribute* a3 = new Attribute; 

a2->linkFrom(a1);
a3->linkFrom(a2);
// a2->linkTo(a3);

*a1 = 12;

std::cout << "a1 : " << *a1 <<std::endl;
std::cout << "a2 : " << *a2 <<std::endl;
std::cout << "a3 : " << *a3 <<std::endl;

delete a2;


*a1 = 23;

std::cout << "a1 : " << *a1 <<std::endl;
// std::cout << "a2 : " << *a2 <<std::endl;
std::cout << "a3 : " << *a3 <<std::endl;

} 


// Quand delete owner, delete all dst.link_to
// Quand delete not owner, delete src.link_from