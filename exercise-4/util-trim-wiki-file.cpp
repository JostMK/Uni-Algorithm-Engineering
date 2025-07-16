//
// Created by Jost on 16/07/2025.
//

#include <fstream>
#include <string>

const std::string WIKI_FILE = "dewiki-20220201-clean.txt";
const std::string OUT_FILE = "dewiki-trim-";
constexpr int ARTICLE_COUNT = 10;

int main() {
    std::ifstream data_file(WIKI_FILE);

    std::ofstream out_file(OUT_FILE + std::to_string(ARTICLE_COUNT) + ".txt");

    int article_count = 0;
    std::string line;
    while (std::getline(data_file, line)) {
        if (line.empty()) {
            article_count++;

            if (article_count == ARTICLE_COUNT)
                break;
        }
        out_file << line << "\n";
    }
    out_file << "\n" << std::endl;

    data_file.close();
    out_file.close();
}
