
#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <string>
#include "util.h"
#include <vector>

using namespace std;

vector<vector<string>> vals; 
vector<bool> instruction;

class Parse{
    public:
    
    /*
     * returns true if request is store, false if load
     * will read the address from the request and put value into @addr
     * if it is a store operation, will put value into @value
     */
    bool fetch_request(string x, u32 &addr, u32 &value){
            char l;
            length = x.length();
        string integer;
        string store;
            
        if(x[0] == 'l'){
        l = x[0];
            for(int i = 0; i < length; i++){
            if(x[i] == 'x'){
            for(int j = i + 1; i < length; i++){
                if(isalpha(x[j])){
                integer += hex2num(x[j]);
                }
                else {
                integer += x[j];
                }
            }
            }

        }
         }
         else if(x[0] == 's'){
        l = x[0];
        for(int i = 4; i < length; i++){
            if(x[i] == 'x'){
            for(int j = i + 1; i <length; i++){
                if(isalpha(x[j])){
                store += hex2num(x[j]);
                }
                else{
                store += hex2num(x[j]);
                }
            }
            brake;
            }
            else{
            integer += hex2num(x[i]);
            }
        }
          
         }

         if(l == 'l'){
        addr = integer;
        return false;
         }
         else{
        addr = integer;
        value = store;
        return true;
         }
    }

};

#endif
