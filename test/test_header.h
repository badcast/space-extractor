/*
 * author: badcast <lmecomposer@gmail.com>
 * year 2023
 * status: already coding
*/

#ifndef HEADER_SOURCE_H
#define HEADER_SOURCE_H

#include <iostream>

#define private public
#define protected public
#include <across.hpp>
#undef private
#undef protected


using namespace std;
using namespace across;

Brain map = across::Brain(100, 100); // sizes with 10 * 10 = 100 neurons

std::size_t maxIterate = map.size();

Brain::neuron_type *n1, *n2;
Brain::site_type t1, t2;
std::size_t iterate = 0;
std::size_t errors = 0;

#endif