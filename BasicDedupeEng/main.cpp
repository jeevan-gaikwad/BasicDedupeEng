// BasicDedupeEng.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <openssl/sha.h>

#include "DedupeEngine.h"
#include "Logging.h"

void printMsg(std::ostringstream str) {
    std::cout << str.str()<<std::endl;
}
template<typename... Args>
void logMe(Args... args) {

    (std::cout << ... << args) << std::endl;
}


int main()
{
    logmsg(INFO, "-----------------------------------------------------------------------------------");
    logmsg(INFO, "-------------------------Basic Dedupe Engine execution started---------------------");
    logmsg(INFO, "-----------------------------------------------------------------------------------");
    DedupeEngine eng;
    std::cout << "Size of dedupemeta data is:" << sizeof(DedupedDataInfo) << std::endl;

    const std::string inputFilename = "XP-PENWin_3.2.2.211027.zip";

    DedupedDataInfo dedupeData;

    //eng.dedupeFile(inputFilename);
    logmsg(INFO, "Deduplication is complete. now testing recovery..");
    eng.createFileFromEngine(inputFilename);

    std::cout << "Done" << std::endl;
    
    /*
    logmsg(INFO, "Test log msg");
    logmsg(DEBUG, "this is another log msg");
    logmsg(ERROR, "this is another another log msg");
    */
    //printMsg(std::ostringstream() << "Hello there");
    //printMsg(STR2(std::ostringstream()<<"Hello my dear"<<i));
    
    /*
    unsigned char ibuf[] = "compute sha1";
    unsigned char obuf[20];

    SHA1(ibuf, strlen((char*)ibuf), obuf);

    int i;
    std::string output;
    for (i = 0; i < 20; i++) {
        char arr[10];
        sprintf(arr, "%02x", obuf[i]);
        output += arr;
    }
    std::cout << "Checksum is:" << output << std::endl;
    printf("\n");
    */
    logmsg(INFO, "-------------------------Basic Dedupe Engine execution end---------------------");
}
