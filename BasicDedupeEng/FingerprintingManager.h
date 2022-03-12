#pragma once
#include "Common.h"
#include <iostream>
#include "openssl/sha.h"

class FingerprintingManager
{

public:
	bool isFingerprintPresentInCache(const std::string& fingerprint);
	void addFingerprintToCache(const std::string& fingerprint);
	void removeFingerprintFromCache(const std::string& fingerprint);
	status_t calculateFingerprint(const  Data& data, DedupedDataInfo&);
private:
	//status_t buildFingerprintCacheFromDisk();
};