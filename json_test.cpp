#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main(){

    std::string newFile = "";

    for (int i = 1; i < 101; i++) {
        newFile = "C:\\Users\\casar\\OneDrive\\Desktop\\json_test\\" + std::to_string(i);
        newFile += ".txt";
        std::cout << "" << std::endl;
        std::cout << "" << std::endl;
        std::cout << std::to_string(i) << std::endl;

        std::ifstream file(newFile);
        if (!file.is_open()) {
            std::cout << "Failed to open the file." << std::endl;
            return 1;
        }
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        json data;

        
        //std::cout << data << std::endl;
        try
        {
            ss >> data;
            std::cout << data;
            std::cout << std::to_string(i);
        }
        catch (json_exception c)
        {
            std::cout << "JSON errato";
        }

    }
    std::cout << "Il parsing e' andato a buon fine" << std::endl;
    return 0;
} 