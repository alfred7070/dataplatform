#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>

void Error(const char* msg);
void Warn(const char* msg);
void Info(const char* msg);
void Info(std::string msg);

#endif
