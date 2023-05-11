#pragma once

#include <iostream>
#include <Windows.h>
#include <future>

void print(const char* fmt, const char* msg = "");
void printAndExit(const char* func, int exitCode = -1);
std::string format(const double value);

void load_AngleApi();
int main_AngleApp();

void load_RangeApi();
int main_RangeApp();

// Api0, Api �� ��ũ��Ʈ ������ ������.
typedef bool(__cdecl* Api0)();
typedef bool(__cdecl* Api)(const char*);

