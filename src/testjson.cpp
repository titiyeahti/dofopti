#include <cstdlib>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std;

int main(){
	ifstream ifs("../data/items.json");
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj);
	for(unsigned int i = 0; i < obj.size(); i++){
		cout << obj[i]["name"]["fr"].asString() << endl;
		cout << obj[i]["name"]["en"].asString() << endl;
	}

	ifs.close();
	return 0;
}
