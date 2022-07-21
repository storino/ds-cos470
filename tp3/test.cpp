#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main() {

    ofstream myfile;
    myfile.open ("resultados.txt", ios_base::app);
    myfile << "Writing this to a file.\n";
    myfile << "Writing this to a file.\n";
    myfile.close();

    return 0;
}

