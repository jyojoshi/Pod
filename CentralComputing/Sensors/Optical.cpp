#include "../Sensor_Package.h"
#include <iostream>

using namespace std;

Optical::Optical(Sensor_Configuration configuration) : Sensor_Group(configuration) {
	
	first_index = OPTICAL_INDEX; // index offset to read from spi
	device = XMEGA1; //xmega device number (0 or 1)
	count = 2; //number of sensors
    translation_array = {{0.0018310542,1}};
    name = "Optical";
    name_array = {{"RPM", "Count"}};
	data = vector<double>(count);		
}

Optical::~Optical(){
	//do nothing
}

void Optical::reset() {
	//TODO: implement resetting
}

void Optical::update(Spi * spi) {

	switch(simulation) {
		case 0:
			refresh_data(spi);
			break;
		case 1:
			simulation_1();
			break;
	}
}

void Optical::simulation_1() {
	auto start = Sensor_Package::start_time;
	auto now = Sensor_Package::get_current_time();
	auto difference = now - start;
	
	//TODO calc some actual value
	
	sensor_group_mutex.lock();
	for(size_t i = 0; i < data.size(); i++) {
		data[i] = i;
	}
	sensor_group_mutex.unlock();
}