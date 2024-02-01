#include<iostream>
#include"../include/Editor.h"

int main(int argc, char **argv) {
    char *f = NULL;
    if(argc > 0) {
        f = argv[1];
    }
    bool is = mono::run(f);
    return 0;
}