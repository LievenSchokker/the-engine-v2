#pragma once


/**
 * @brief Specifies how a message is delivered over the network.
 *
 * The message is sent using the specified SendMode:
 * - **ReliableOrdered**: Guarantees reliable, in-order delivery.
 * - **ReliableUnordered**: Guarantees delivery but may arrive out of order.
 * - **Unreliable**: Delivery is not guaranteed; messages may be lost or arrive out of order.
 */
enum class SendMode {
    ReliableOrdered,
    ReliableUnordered,
    Unreliable
};
