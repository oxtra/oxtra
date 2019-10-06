#include "logger.h"

static uint8_t logging_level = 0;

uint8_t logger::get_level() {
	return logging_level;
}

bool logger::get_riscv() {
	return (logging_level & Level::riscv) > 0;
}

bool logger::get_x86() {
	return (logging_level & Level::x86) > 0;
}

bool logger::get_translated() {
	return (logging_level & Level::translated) > 0;
}

bool logger::get_reroutes() {
	return (logging_level & Level::reroutes) > 0;
}

bool logger::get_syscall() {
	return (logging_level & Level::syscall) > 0;
}

bool logger::get_return_value() {
	return (logging_level & Level::return_value) > 0;
}

void logger::set_level(uint8_t set) {
	logging_level = set;
}

void logger::set_riscv(bool set) {
	logging_level = set ? (logging_level | Level::riscv) : (logging_level & ~Level::riscv);
}

void logger::set_x86(bool set) {
	logging_level = set ? (logging_level | Level::x86) : (logging_level & ~Level::x86);
}

void logger::set_translated(bool set) {
	logging_level = set ? (logging_level | Level::translated) : (logging_level & ~Level::translated);
}

void logger::set_reroutes(bool set) {
	logging_level = set ? (logging_level | Level::reroutes) : (logging_level & ~Level::reroutes);
}

void logger::set_syscall(bool set) {
	logging_level = set ? (logging_level | Level::syscall) : (logging_level & ~Level::syscall);
}

void logger::set_return_value(bool set) {
	logging_level = set ? (logging_level | Level::return_value) : (logging_level & ~Level::return_value);
}