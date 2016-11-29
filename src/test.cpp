#include <fstream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

int main() {
    ifstream myfile ("config.json");
    if (myfile.is_open())
    {
        string content( (std::istreambuf_iterator<char>(myfile) ),
                (std::istreambuf_iterator<char>()    ) );
        json configJson = json::parse(content);
        cout << configJson["backup"];
        myfile.close();
    }
    else cout << "Unable to open file"; 
    return 0;
}
