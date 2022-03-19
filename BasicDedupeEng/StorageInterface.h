#pragma once

/* Intent of this interface would be to abstract checksum, index file & data file data storage interface
*  Irrespective of storage mechanism like File system based, DB, network based storage, Dedupe Engine should
*  talk to the same Storage/Network interface
*/