
#include <iostream>
#include "ffmpegUtil.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;

extern void playVideoWithAudio(const string& inputPath);


int main() {

    cout << "input midia file:" << endl;
    string inputPath;
    cin >> inputPath;


    cout << "play file: " << inputPath << endl;
    cout << "----------------------------------------------------------------------" << endl;
    playVideoWithAudio(inputPath);
    return 0;
}
