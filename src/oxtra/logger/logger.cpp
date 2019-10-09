#include "logger.h"

static uint8_t logging_level = 0;

uint8_t logger::get_level() {
	return logging_level;
}

bool logger::get_level(logger::Level::Value l) {
	return (logging_level & l) != 0;
}

void logger::set_level(uint8_t set) {
	logging_level = set;
}

void logger::set_level(logger::Level::Value l, bool state) {
	if (state) {
		logging_level |= l;
	} else {
		logging_level &= ~l;
	}
}

void logger::reroutes(const char* str, uintptr_t address) {
	if (!get_level(Level::reroutes))
		return;

	fmt::print(str, address);
}