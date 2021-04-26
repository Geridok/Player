#pragma once

#include <string>
#include <iostream>
#include <fstream>

extern double c_1;
extern double c_2;
extern double c_3;
extern size_t partSize;
extern size_t errorAmount;

class workWithIniFile {
public:

	static std::string fileIniName;

	static void reworkIniFile(double recivedC_1, double recivedC_2, double recivedC_3,size_t recivedPartSize,size_t recivedErrorAmount) {
		std::ofstream fileId;
		fileId.open(workWithIniFile::fileIniName, std::ios::out | std::ios::trunc);

		fileId << "c_1 = " << recivedC_1 << "\n";
		fileId << "c_2 = " << recivedC_2 << "\n";
		fileId << "c_3 = " << recivedC_3 << "\n";
		fileId << "partSize = " << recivedPartSize << "\n";
		fileId << "errorAmount = " << recivedErrorAmount << "\n";
	}

	static bool readIniFile() {
		std::ifstream fileId(workWithIniFile::fileIniName, std::ios::in);
		if (!fileId.is_open()) {
			return false;
		}
		std::string line;

		while (std::getline(fileId, line)) {
			auto foudedIndex = line.find_first_of("=");
			if (foudedIndex == std::string::npos) {
				return false;
			}
			std::string name = line.substr(0, foudedIndex - 1);
			std::string value = line.substr(foudedIndex + 2);
			writeToGlobal(name, value);
		}
	}

	static void writeToGlobal(std::string name,std::string value) {

		if (name == "c_1") {
			c_1 = std::stod(value);
		}else  if(name == "c_2") {
			c_2 = std::stod(value);
		}
		else if(name == "c_3") {
			c_3 = std::stod(value);
		}
		else if (name == "partSize"){
			partSize = std::stoul(value);
		}
		else if (name == "errorAmount") {
			errorAmount == std::stoul(value);
		}
	}
};