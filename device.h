//
//
//	device.h
//
//	(C) R.P.Bellis 2021
//
//

#pragma once

#include <vector>
#include "typedefs.h"

/*
 * an abstract memory mapped device
 */
class Device {

public:
	virtual void		reset(void) = 0;
	virtual void		tick(void) = 0;

public:
	virtual Byte		read(Word offset) = 0;
	virtual void		write(Word offset, Byte val) = 0;

};

/*
 * a mapping from memory locations to Devices
 */
struct DeviceEntry {
	Device*			device;
	Word			base;
	Word			mask;
};

typedef std::vector<DeviceEntry> Devices;
