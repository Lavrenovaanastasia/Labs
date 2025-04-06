#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>
#include <random>
#include <vector>

using namespace std;

string escapeRegexString(const string& s) {
    static const regex specialChars{ R"([-[\]{}()*+?.,\^$|#\s])" };
    return regex_replace(s, specialChars, R"(\$&)");
}

string generateRandomName(mt19937& rng) {
    static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uniform_int_distribution<int> len_dist(8, 12);
    uniform_int_distribution<int> char_dist(0, sizeof(chars) - 2);

    int length = len_dist(rng);
    string name;
    for (int i = 0; i < length; ++i) {
        name += chars[char_dist(rng)];
    }
    return name;
}

void normalizeLineEndings(string& line) {
    line.erase(remove(line.begin(), line.end(), '\r'), line.end());
    line += "\r\n";  
}

void obfuscateCode(const string& input_path, const string& output_path) {
    ifstream input_file(input_path);
    vector<string> lines;
    string line;

    while (getline(input_file, line)) {
        lines.push_back(line);
    }
    input_file.close();

    ofstream output_file(output_path, ios::out | ios::trunc);

    for (const auto& line : lines) {
        if (line.find("#include") == 0) {
            string processed_line = line;
            normalizeLineEndings(processed_line); 
            output_file << "# " << processed_line << endl;
        }
    }

    unordered_map<string, string> replacements;
    vector<string> macro_defs;
    int macro_counter = 1;
    regex string_regex(R"("(\\.|[^"\\])*")");
    regex comment_regex(R"(\/\/.*)");

    // для макросов
    for (const auto& line : lines) {
        if (!line.empty() && line.find("#") == 0) continue;

        sregex_iterator s_it(line.begin(), line.end(), string_regex);
        sregex_iterator s_end;
        while (s_it != s_end) {
            smatch match = *s_it;
            string str = match.str();
            if (replacements.find(str) == replacements.end()) {
                string macro = "a" + to_string(macro_counter++);
                replacements[str] = macro;
                macro_defs.push_back("#define " + macro + " " + str);
            }
            ++s_it;
        }

        sregex_iterator c_it(line.begin(), line.end(), comment_regex);
        sregex_iterator c_end;
        while (c_it != c_end) {
            smatch match = *c_it;
            string comment = match.str();
            if (replacements.find(comment) == replacements.end()) {
                string macro = "a" + to_string(macro_counter++);
                replacements[comment] = macro;
                macro_defs.push_back("#define " + macro + " " + comment);
            }
            ++c_it;
        }
    }

    
    for (const auto& def : macro_defs) {
        string processed_line = def;
        normalizeLineEndings(processed_line);  
        output_file << processed_line << endl;
    }

    //  идентификатор
    unordered_map<string, string> name_map;
    random_device rd;
    mt19937 rng(rd());
    regex identifier_regex(R"(\b[a-zA-Z_][a-zA-Z0-9_]*\b)");
    regex type_regex(R"(\b(int|double|long|long long|include|iostream|float|char|bool|std|cout|main|return|void|endl|string|auto)\b)");
    regex spaces_regex(R"( +)");
    regex macro_regex(R"(^a\d+$)");

    for (string line : lines) {
        if (line.find("#") == 0 && line.find("#include") != 0) {
            string processed_line = line;
            normalizeLineEndings(processed_line);  
            output_file << processed_line << endl;
            continue;
        }

        // Замена строк и комментариев
        string processed = line;
        for (const auto& repl : replacements) {
            string escaped_pattern = escapeRegexString(repl.first);
            processed = regex_replace(processed, regex(escaped_pattern), repl.second);
        }

        sregex_iterator it(processed.begin(), processed.end(), identifier_regex);
        vector<pair<string, string>> id_repls;

        while (it != sregex_iterator()) {
            smatch match = *it;
            string var = match.str();

            if (regex_match(var, macro_regex) || regex_match(var, type_regex)) {
                ++it;
                continue;
            }

            if (name_map.find(var) == name_map.end()) {
                name_map[var] = generateRandomName(rng);
            }
            id_repls.emplace_back(var, name_map[var]);
            ++it;
        }

        for (const auto& repl : id_repls) {
            string escaped_var = escapeRegexString(repl.first);
            processed = regex_replace(processed, regex(R"(\b)" + escaped_var + R"(\b)"), repl.second);
        }

        processed = regex_replace(processed, spaces_regex, "   ");
        normalizeLineEndings(processed);  
        output_file << processed << endl;
    }

    output_file.close();
}

int main() {
    const string input_path = "D:/BGYIR/3kyrs/6sem/ISOB/L7/L7/test/test.cpp";
    const string output_path = "D:/BGYIR/3kyrs/6sem/ISOB/L7/L7/obfuscated/obfuscated.cpp";

    obfuscateCode(input_path, output_path);
    cout << "Obfuscation complete " << output_path << endl;

    system("pause");
    return 0;
}
