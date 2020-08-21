#pragma once

#include <atomic>
#include <map>
#include <vector>
#include "execution_graph/logic_controllers/CacheMachine.h"
#include <blazingdb/manager/Context.h>
#include "kernel.h"

namespace ral {
namespace cache {

using Context = blazingdb::manager::Context;

/**
    @brief The distributing_kernel interface allows kernels calling distributing primitives.
*/
class distributing_kernel : public kernel {
    public:
    /**
     * Constructor for the distributing_kernel
     * @param kernel_id Current kernel identifier.
     * @param expr Original logical expression that the kernel will execute.
     * @param context Shared context associated to the running query.
     * @param kernel_type_id Identifier representing the kernel type.
     */
    distributing_kernel(std::size_t kernel_id,
        std::string expr,
        std::shared_ptr<Context> context,
        kernel_type kernel_type_id);

    /**
     * @brief Resizes the vector of the message trackers.
     *
     * @param num_message_trackers The new number of the message trackers.
     */
    void set_number_of_message_trackers(std::size_t num_message_trackers);

    /**
     * @brief Sends a table with their corresponding metadata.
     *
     * @param table The table to be sent. If table is a nullptr, an empty table is sent anyways.
     * @param specific_cache Indicates if a message should be routed to a specific cache or to the global input cache.
     * @param cache_id Indicates what cache a message should be routed to.
     * @param target_id Indicates what worker is sending this message.
     * @param total_rows Indicates how many rows are in this message.
     * @param message_id_prefix The prefix of the identifier of this message.
     * @param always_add Forces to always add the table to the output cache.
     * @param wait_for Indicates if this message must be registered to wait for back.
     * @param message_tracker_idx The message tracker index.
     * @param extra_metadata The cache identifier.
     */
    void send_message(std::unique_ptr<ral::frame::BlazingTable> table,
        std::string specific_cache,
        std::string cache_id,
        std::string target_id,
        std::string total_rows = "",
        std::string message_id_prefix = "",
        bool always_add = false,
        bool wait_for = false,
        std::size_t message_tracker_idx = 0,
        ral::cache::MetadataDictionary extra_metadata = {});

    /**
     * @brief Sends each partition to its corresponding nodes.
     * It is assumed that the size of the vector is the same as the number of nodes.
     *
     * @param partitions The table partitions to be sent.
     * @param output The output cache.
     * @param graph_output The output graph.
     * @param message_id_prefix The prefix of the identifier of this message.
     * @param cache_id Indicates what cache a message should be routed to.
     * @param message_tracker_idx The message tracker index.
     */
    void scatter(std::vector<ral::frame::BlazingTableView> partitions,
        ral::cache::CacheMachine* output,
        ral::cache::CacheMachine* graph_output,
        std::string message_id_prefix,
        std::string cache_id,
        std::size_t message_tracker_idx = 0);

    /**
     * @brief Sends the partition counter to all other nodes.
     *
     * @param graph_output The output graph.
     * @param message_id_prefix The prefix of the identifier of this message.
     * @param cache_id Indicates what cache a message should be routed to.
     * @param message_tracker_idx The message tracker index.
     */
    void send_total_partition_counts(ral::cache::CacheMachine* graph_output,
        std::string message_id_prefix,
        std::string cache_id,
        std::size_t message_tracker_idx = 0);

    /**
     * @brief Get the total partition counters associated to a message tracker.
     *
     * @param message_tracker_idx The message tracker index.
     */
    int get_total_partition_counts(std::size_t message_tracker_idx = 0);

    /**
     * @brief Increments by one the corresponding node counter associated to
     * the node identifier and the message tracker identifier.
     *
     * @param node_id The node identifier.
     * @param message_tracker_idx The message tracker index.
     */
    void increment_node_count(std::string node_id, std::size_t message_tracker_idx = 0);

    /**
     * Destructor
     */
    virtual ~distributing_kernel() = default;

    private:
        const blazingdb::transport::Node& node; /**< Stores the reference of the current node. */
        std::vector<std::map<std::string, std::atomic<size_t>>> node_count; /**< Vector of maps that stores the message count associated to a node. Each vector corresponds to a message tracker. It's thread-safe. */
        std::vector<std::vector<std::string>> messages_to_wait_for; /**< Vector of vectors of the messages registered to wait for. Each vector corresponds to a message tracker. It's thread-safe. */
};

}  // namespace cache
}  // namespace ral
