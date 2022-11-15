#include "RedArchiver.hpp"
#include "archiver.hpp"

#include <algorithm>

// Author: Sorry for my french...

using haff_tree_size_t = uint64_t;
using num_bits_t = uint64_t;

int archive(void *_src, int srcSizeBytes, void *&_dest, int &destSizeBytes)
{
	auto *src = (uint8_t *)_src;
	auto *dest = (uint8_t *)_dest;

	// Contract to use src and dest
	static_assert(sizeof(*src) == 1);
	static_assert(sizeof(*dest) == 1);

	// Archivate
	archiver::ArchiverCPU arch;
	auto [archived_data, num_bits, haff_tree] = arch.archive(src, src + srcSizeBytes, 0, 255);

	// Calc full size
	const int code_table_size = sizeof(haff_tree_size_t) + haff_tree.size() * sizeof(archiver::node_t);
	const int archive_size = sizeof(num_bits_t) + archived_data.size();
	destSizeBytes = code_table_size + archive_size;

	_dest = (uint8_t *)malloc(destSizeBytes);
	dest = (uint8_t *)_dest;

	// Serialize code table data (Haff-tree and size it)
	*((haff_tree_size_t *)dest) = haff_tree.size();
	dest += sizeof(haff_tree_size_t);
	dest = (uint8_t*)std::copy(haff_tree.cbegin(), haff_tree.cend(), (archiver::node_t*)dest);

	// Serialize data
	*((num_bits_t *)dest) = num_bits;
	dest += sizeof(num_bits_t);
	std::copy(archived_data.cbegin(), archived_data.cend(), dest);

	return 0;
}

int unarchive(void *_src, int srcSizeBytes, void *&_dest, int &destSizeBytes)
{
	auto *src = (uint8_t *)_src;
	auto *dest = (uint8_t *)_dest;

	// Contract to use src and dest
	static_assert(sizeof(*src) == 1);
	static_assert(sizeof(*dest) == 1);
	
	// Deserialize haff tree
	const auto haff_tree_size = *(haff_tree_size_t *)src;
	src += sizeof(haff_tree_size_t);

	std::vector<archiver::node_t> haff_tree(haff_tree_size);
	std::copy_n((const archiver::node_t*)src, haff_tree_size, haff_tree.data());
	src += haff_tree_size * sizeof(archiver::node_t);

	// Deserialize data
	const auto num_bits = *(num_bits_t *)src;
	src += sizeof(num_bits_t);

	std::vector<archiver::data_t> archived_data(archiver::num_bits2num_bytes(num_bits));
	std::copy_n(src, archived_data.size(), archived_data.data());

	// Dearchivate
	archiver::ArchiverCPU arch;
	const std::vector<archiver::data_t> data_decoded = arch.dearchive(archived_data, num_bits, haff_tree, 0);

	destSizeBytes = data_decoded.size();
	dest = (uint8_t*)malloc(data_decoded.size() * sizeof(data_decoded[0]));
	std::copy_n(data_decoded.data(), destSizeBytes, (uint8_t *)dest);

	_dest = dest;

	return 0;
}
