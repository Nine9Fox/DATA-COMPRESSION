#ifndef BWTSEARCH_H
#define BWTSEARCH_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <experimental/filesystem>

struct bwtSearchRs {
    unsigned int first;
    unsigned int last;
};

std::map<char, unsigned int> generate_CTable(char* bwt, unsigned int len);
unsigned int Occ(const char* bwt, char c, unsigned int offset);
unsigned int Occ(const char* bwt, char c, unsigned int offset, unsigned int* start, unsigned int* count);
bwtSearchRs backwardSearch(const char* pattern, std::map<char, unsigned int> c_Table, const char* bwt);
std::string backwardDecode(std::map<char, unsigned int> c_Table, const char* bwt, unsigned int offset, char start);
unsigned int backwardDecodeIndex(std::map<char, unsigned int> c_Table, const char* bwt, unsigned int offset, char c);
std::string forwardDecode(std::map<char, unsigned int> c_Table, const char* bwt, unsigned int offset, char c);
void saveC_TableToIndexFile(const std::string& filename, const std::map<char, unsigned int>& map);
std::map<char, unsigned int> readC_TableToIndexFile(const std::string& filename);

#endif