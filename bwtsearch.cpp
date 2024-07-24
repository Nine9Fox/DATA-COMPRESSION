#include "bwtsearch.h"

std::map<char, unsigned int> generate_CTable(const char* bwt, unsigned int len) {
    std::map<char, unsigned int> c_Table;

    for (unsigned int i = 0; i < len; ++i) {
        c_Table[bwt[i]]++;
    }

    unsigned int count = 0;
    for (auto& pair : c_Table) {
        unsigned int old_count = pair.second;
        pair.second = count; 
        count += old_count;  
    }

    c_Table[127] = count;

    return c_Table;
}

unsigned int Occ(const char* bwt, char c, unsigned int offset, unsigned int* start, unsigned int* count) {
    unsigned int res = *count;
    for (unsigned int i = *start; i < offset; i++) {
        if (bwt[i] == c) {
            res++;
        }
    }
    *count = res;
    *start = offset;
    return res;
}

unsigned int Occ(const char* bwt, char c, unsigned int offset) {
    unsigned int res = 0;
    for (unsigned int i = 0; i < offset; i++) {
        if (bwt[i] == c) {
            res++;
        }
    }
    return res;
}

bwtSearchRs backwardSearch(const char* pattern, std::map<char, unsigned int> c_Table, const char* bwt) {

    unsigned int i = strlen(pattern) - 1;
    char c = pattern[i];
    bwtSearchRs result;

    auto it = c_Table.find(c);

    if (it == c_Table.end()) {
        result.first = (c_Table.end()->second) + 1;
        result.last = c_Table.end()->second;
    } else {
        result.first = (it->second) + 1; 
        it = std::next(it);
        result.last = it->second;
    }

    unsigned int start = 0; 
    unsigned int count = 0;

    while ((result.first <= result.last) && (i + 1 >= 2)) {
        
        start = 0;
        count = 0;
        c = pattern[i - 1];
        auto it = c_Table.find(c);

        if (it == c_Table.end()) {
            result.first = (c_Table.end()->second) + 1;
            result.last = c_Table.end()->second;
        } else {
            result.first = (it->second) + Occ(bwt, c, result.first - 1, &start, &count) + 1;
            result.last = (it->second) + Occ(bwt, c, result.last, &start, &count);
        }
        i--;
    }

    if (result.first > result.last) {
        result.first = c_Table.end()->second;
        result.last = c_Table.end()->second;
    } 
    return result;
}

std::string backwardDecode(std::map<char, unsigned int> c_Table, const char* bwt, unsigned int offset, char c) {
    std::string curr;

    while (1) {
        c = bwt[offset];
        curr.insert(0, 1, c);
        if (c == '[') break;
        offset = c_Table[c] + Occ(bwt, c, offset);   
    }

    return curr;
}

unsigned int backwardDecodeIndex(std::map<char, unsigned int> c_Table, const char* bwt, unsigned int offset, char c) {
    unsigned int cur, res, mul;
    mul = 1;
    res = 0;
    bool isIndex = 0;
    while (1) {
        
        if (isIndex == 1) {

            c = bwt[offset];                         
            if (c == '[') break;            
            cur = c - '0';
            res += cur * mul;
            mul *= 10;
            offset = c_Table[c] + Occ(bwt, c, offset);
        } else {
            c = bwt[offset];
            if (c == '[') break;
            if (c == ']') isIndex = 1;                
            offset = c_Table[c] + Occ(bwt, c, offset);            
        }

    }
    return res;
}

std::string forwardDecode(std::map<char, unsigned int> c_Table, const char* bwt, unsigned int offset, char c) {
   
    std::string curr;
    unsigned int num;
    unsigned int count;

    curr.push_back(c); 

    while (1) {
        num = offset - c_Table[c] + 1;
        count = 0;
        unsigned int i = 0;

        for (; i < strlen(bwt); i++) {
            if (count == num) {
                break;
            }

            if (bwt[i] == c) {
                count++;
            }
        }
        i--;

        auto it = c_Table.begin();
        for (; it != c_Table.end(); ++it) {

            if (i == it->second) {
                break;
            } else if (i < it->second) {
                --it;
                break;
            }
        }

        c = it->first;
        offset = i;
        if (c == '[') break;
        curr.push_back(c);    
    }

    return curr;
}

void saveC_TableToIndexFile(const std::string& filename, const std::map<char, unsigned int>& map) {
    std::ofstream file(filename, std::ios::binary);

    uint32_t size = map.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& pair : map) {
        file.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
        file.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }

    file.close();
}

std::map<char, unsigned int> readC_TableToIndexFile(std::ifstream& file) {
    std::map<char, unsigned int> map;

    uint32_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    for (unsigned int i = 0; i < size; i++) {
        char key;
        unsigned int value;
        file.read(reinterpret_cast<char*>(&key), sizeof(key));
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        map[key] = value;
    }

    return map;
}

int main(int argc, char *argv[]) {
    std::fstream bwt_InputFile;
    bwt_InputFile.open(argv[1], std::ios::in);

    std::string bwt;
    std::map<char, unsigned int> c_Table;
    std::map<unsigned int, std::string> strings_Result;

    char c;
    int bwt_len = 0;
    while (bwt_InputFile.get(c)) {
        bwt.push_back(c);       
        bwt_len++;
    }

    std::string index_FileName = argv[2];    
    std::ifstream index_File(index_FileName, std::ios::binary);

    if (!index_File.is_open()) {
        c_Table = generate_CTable(bwt.data(), bwt_len);
        std::size_t bwt_Size = bwt.size();
        std::size_t c_Table_Size = c_Table.size();

        if (bwt_Size > ((c_Table_Size) * 12)) {
            saveC_TableToIndexFile(index_FileName, c_Table);        
        }    
    } else {
        c_Table = readC_TableToIndexFile(index_File);
    }

    index_File.close();

    bool isFirst = 1;
    for (int pa_Count = 3; pa_Count < argc; pa_Count++) {
        std::map<unsigned int, std::string> strings_Curr;
        char* pattern = argv[pa_Count];

        bwtSearchRs result = backwardSearch(pattern, c_Table, bwt.data());
        bwtSearchRs result_Final;

        for (unsigned int i = result.first - 1; i < result.last; i++) {   
          
            unsigned index = backwardDecodeIndex(c_Table, bwt.data(), i, pattern[0]) + 1;

            std::string index_Str = "[" + std::to_string(index) + "]";
            result_Final = backwardSearch(index_Str.data(), c_Table, bwt.data());

            std::string curr;
            if (result_Final.first == c_Table.end()->second) {
                curr = backwardDecode(c_Table, bwt.data(), i, pattern[0]);
                curr += forwardDecode(c_Table, bwt.data(), i, pattern[0]);

            } else {
                curr = backwardDecode(c_Table, bwt.data(), result_Final.first - 1, '[');
            }

            strings_Curr[index] = curr;
            if (isFirst) {
                strings_Result[index] = curr;
            }
        }

        std::map<unsigned int, std::string> intersection;
        std::set_intersection(
            strings_Curr.begin(), strings_Curr.end(),
            strings_Result.begin(), strings_Result.end(),
            std::inserter(intersection, intersection.begin()),
            [](const std::pair<unsigned int, std::string>& a, const std::pair<unsigned int, std::string>& b) {
            return a.first < b.first;
            }
        );
        strings_Result = intersection;
        isFirst = 0;
    }

    for (auto& str : strings_Result) {
        std::cout << str.second << std::endl;
    }

}    