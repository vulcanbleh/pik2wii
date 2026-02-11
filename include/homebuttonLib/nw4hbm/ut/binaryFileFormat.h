#ifndef NW4HBM_UT_BINARY_FILE_HEADER_H
#define NW4HBM_UT_BINARY_FILE_HEADER_H

#include <types.h>

namespace nw4hbm { 
namespace ut {

struct BinaryFileHeader {
	byte4_t	signature;	// _00
	byte2_t	byteOrder;	// _04
	u16	version;		// _06
	u32	fileSize;		// _08
	u16	headerSize;		// _0C
	u16	dataBlocks;		// _0E
};

struct BinaryBlockHeader {
	byte4_t	kind;	// _00
	u32	size;		// _04
};

	bool IsValidBinaryFile(BinaryFileHeader const *header, byte4_t signature,
	                       u16 version, u16 minBlocks);
	bool IsReverseEndianBinaryFile(BinaryFileHeader const *fileHeader);
	BinaryBlockHeader *GetNextBinaryBlockHeader(BinaryFileHeader *fileHeader,
	                                            BinaryBlockHeader *blockHeader);

} // namespace ut
} // namespace nw4hbm

#endif // NW4HBM_UT_BINARY_FILE_HEADER_H
