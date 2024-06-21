#include "demodll.h"

DemoDLL::DemoDLL() {
    cout << "Init DemoDLL << endl";
}

void DemoDLL::PrintNUM()
{
    for(int i = 0; i < 100000; i++){
        cout << "num: " << i << endl;
    }
}
