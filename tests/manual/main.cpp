#include <iostream>
#include "../../include/linda.h"
#include "../../include/tuple.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include "../../include/tuple_pattern.h"
#include <stdio.h>

using namespace std;

//#define WRITER
#define READER

int main(int argc, char** argv)
{
    srand(time(NULL));

    cout << "Begin" << endl;
    Linda l;
    cout << "created"<<endl;
    cout << "init_result:"<<l.init(5678)<<endl; // ftok() ?

    l.setDebugMode(true);

    Tuple<int, float, std::string> tu(41, 10.0, "ala");
    Tuple<float, float, std::string> tu2(41.2, 10.0, "ala");
    TuplePattern<int, float, std::string> tp("*", "10.0", "*");
    TuplePattern<float, float, std::string> tp2("*", "10.0", "*");

    char line[255];
    char* num;

    while(1) 
    {
        num = fgets(line, 253, stdin);
        if (num == NULL || num[0] == '\n') break;

        switch(line[0])
        {
            case 'w':
            {
                l.output(tu);
                break;
            }
            case 'i':
            {
                try
                {
                    Tuple<> res = l.input(tp, 10);
                    cout << (int)(res.get<int>(0))<<std::endl;
                }
                catch (LindaError)
                {
                    cout << "Error" << std::endl;
                }
                break;
            }
            case 'r':
            {
                try
                {
                    Tuple<> res = l.read(tp, 10);
                    cout << (int)(res.get<int>(0))<<std::endl; 
                }
                catch (LindaError)
                {
                    cout << "Error" << std::endl;
                }
                break;
            }
            case 'e':
            {
                l.output(tu2);
                break;
            }
            case 'o':
            {
                try
                {
                    Tuple<> res = l.input(tp2, 10);
                    cout << (float)(res.get<float>(0))<<std::endl;
                }
                catch (LindaError)
                {
                    cout << "Error" << std::endl;
                }
                break;
            }
            case 't':
            {
                try
                {
                    Tuple<> res = l.read(tp2, 10);
                    cout << (float)(res.get<float>(0))<<std::endl; 
                }
                catch (LindaError)
                {
                    cout << "Error" << std::endl;
                }
                break;
            }
        }
        usleep(100000+(rand()%500 * 1000)); // 0.1s - 0.6s
    }
    system("read -p 'Wcisnij cokolwiek aby wyjsc'");

    return 0;
}