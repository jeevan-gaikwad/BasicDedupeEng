# BasicDedupeEng
This project is created to revise, practice new C++ features and play with IO path. Plan to use file IO, multithreading, networking.

Project description:
This is intended to optimize File storage using checksum(fingerprinting) mechanism. This project should help us to reduce data transfer across network, if we have duplicate data 
and optimize file write & storage. I've planned to use all new C++ features till C++20. For logging purpose, I've introduced a specicial professional level Logger to track project
workflow & issues.

This project shall have following features:
* Dedupe files present in a directory using multithreaded engine
* Recover file from the engine
* Use OpenSSL SHA-1 fingerprinting to calculate checksum
* Use Dedupe Engine warming up to build fingerprint cache and data files references at the time of initializaion to speed to dudeplication and recovery time.
* Calculate dudupe ratio per file
* A dedicated generic Logger to log INFO, DEBUG, ERROR messages. 
* Planned to write Unit testing, may be using gMock as I'm not familier with it yet.
