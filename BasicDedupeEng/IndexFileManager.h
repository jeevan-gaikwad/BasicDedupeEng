#pragma once
#include "Common.h"
#include "FileInterfaces.h"

class IndexFileManager
{
public:
	void createOrOpenIndexFile(const std::string& filename);
	void appendFileMetaData(const DedupedDataInfo& dedupeDataInfo);
	status_t readNextIndexFileRecord(DedupedDataInfo& dedupeDataInfo);
	static std::string getIndexFilename(const std::string& filename);
	IndexFileManager(const std::string filename, std::ios_base::openmode fileOpenMode);
	~IndexFileManager();

private:
	void serializeIndexRecord(const DedupedDataInfo& dedupeDataInfo);
	void deserializeIndexRecord(const DedupedDataInfo& dedupeDataInfo);
	std::ofstream indexFileOutStrm_m; //for writing index records
	std::string indFilename_m;
	std::ifstream indexFileReadStrm_m; // for reading index file records
	inline static const std::string indFilenamePrefix_m = "index_files\\";
	inline static const std::string indFilenameSuffix_m = ".index";
};
