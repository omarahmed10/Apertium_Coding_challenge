/*
 * ModestoXML.c++
 *
 *      Author: omar
 */

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

template<class Container>
void split(const std::string& str, Container& cont, char delim = ' ') {
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim)) {
		cont.push_back(token);
	}
}
string trim(const string& str) {
	size_t first = str.find_first_not_of(" \n");
	if (string::npos == first) {
		return str;
	}
	size_t last = str.find_last_not_of(" \n");
	return str.substr(first, (last - first + 1));
}

string converttoXML(string line) {
	string xml = "";

	vector<string> tokens;
	split(line, tokens, '\'');
	for (unsigned int i = 0; i < tokens.size(); i++) {
		string token = tokens[i];
		if (i == 0) {
			xml.append("\t\t\t<program name=\"" + trim(token) + "\">\n");
		} else if (token.find('/') != string::npos) { // file
			vector<string> pathtokens;
			split(token, pathtokens, '/');
			xml.append(
					"\t\t\t\t<file name=\"" + pathtokens[pathtokens.size() - 1]
							+ "\"/>\n");
		}
	}
	xml.append("\t\t\t</program>\n");
	return xml;
}
int main(int argc, char **argv) {
	if (argc < 3) {
		cout
				<< "enter the directory path of the .mode files then the name of the output xml file"
				<< endl;
		return -1;
	}

	const char* input_dir = argv[1];
	string output_mode_xml = argv[2];
	output_mode_xml.append(".xml");

	ofstream xml_file;
	xml_file.open(output_mode_xml.c_str());
	xml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	xml_file << "<modes>\n";

	struct dirent *entry;
	DIR *dir = opendir(input_dir);

	if (dir == NULL) {
		cout << "modes directory file not Found" << endl;
		return -1;
	}
	while ((entry = readdir(dir)) != NULL) {
		string file_name = entry->d_name;

		if (file_name.find(".mode") == string::npos)
			continue;

		string input_mode(input_dir);
		input_mode.append("/");
		input_mode.append(file_name);
		ifstream input_mode_file(input_mode.c_str());
		string line;
		if (input_mode_file.is_open()) {
			string mode_name = file_name.substr(0, file_name.find("."));
			xml_file
					<< "\t<mode name=\"" + trim(mode_name)
							+ "\">\n\t\t<pipeline>\n";

			while (getline(input_mode_file, line, '|')) {
				string xml_lines = converttoXML(line);
				xml_file << xml_lines;
			}

			xml_file << "\t\t</pipeline>\n";
			input_mode_file.close();
		}
//		break;
	}
	closedir(dir);

	xml_file << "</modes>";
	xml_file.close();
	return 0;
}

