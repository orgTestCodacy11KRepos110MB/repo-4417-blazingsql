#include "ArrowCacheData.h"

namespace ral {
namespace cache {

ArrowCacheData::ArrowCacheData(std::shared_ptr<arrow::Table> table, ral::io::Schema schema)
    : CacheData(CacheDataType::ARROW, schema.get_names(), schema.get_data_types(), table->num_rows()), data{std::move(table)} {}

std::unique_ptr<ral::frame::BlazingTable> ArrowCacheData::decache() {
    return std::make_unique<ral::frame::BlazingTable>(std::move(cudf::from_arrow(*data)), this->col_names);
}

size_t ArrowCacheData::sizeInBytes() const {
	return 0;
}

void ArrowCacheData::set_names(const std::vector<std::string> & names) {
	this->col_names = names;
}

ArrowCacheData::~ArrowCacheData() {}

} // namespace cache
} // namespace ral