// Copyright (c) 2019 Fujian(Quanzhou)-HIT Research Institute of Engineering and Technology All rights reserved.


#ifndef ESD_CAN_DRIVE_H
#define ESD_CAN_DRIVE_H

#include <string>
#include <vector>
#include <iostream>
#include "ntcan.h"
#include "gflags/gflags.h"
#include <memory.h>

const int32_t MAX_CAN_PORT = 4;

const int32_t MAX_CAN_SEND_FRAME_LEN = 4;
const int32_t MAX_CAN_RECV_FRAME_LEN = 10;

struct CanFrame {
  /// Message id
  uint32_t id;
  /// Message length
  uint8_t len;
  /// Message content
  uint8_t data[8];
  /// Time stamp
  struct timeval timestamp;

  /**
   * @brief Constructor
   */
  CanFrame() : id(0), len(0), timestamp{0} {
    memset(data, 0, sizeof(data));
  }

  /**
   * @brief CanFrame string including essential information about the message.
   * @return The info string.
   */
};



class EsdCanClient {
 public:
  /**
   * @brief Initialize the ESD CAN client by specified CAN card parameters.
   * @param parameter CAN card parameters to initialize the CAN client.
   * @return If the initialization is successful.
   */

  EsdCanClient() : port_(0), is_started_(false), baud_(NTCAN_BAUD_500) {}
  EsdCanClient(int port, int baud) : port_(port), is_started_(false), baud_(baud) {}

  /**
   * @brief Destructor
   */
  ~EsdCanClient();

  /**
   * @brief Start the ESD CAN client.
   * @return The status of the start action which is defined by
   *         apollo::common::ErrorCode.
   */
  int Start();

  /**
   * @brief Stop the ESD CAN client.
   */
  void Stop();

  /**
   * @brief Send messages
   * @param frames The messages to send.
   * @param frame_num The amount of messages to send.
   * @return The status of the sending action which is defined by
   *         apollo::common::ErrorCode.
   */
  int Send(const std::vector<CanFrame> &frames,
                                 int32_t *const frame_num);

  /**
   * @brief Receive messages
   * @param frames The messages to receive.
   * @param frame_num The amount of messages to receive.
   * @return The status of the receiving action which is defined by
   *         apollo::common::ErrorCode.
   */
  int Receive(std::vector<CanFrame> *const frames,
                                    int32_t *const frame_num);

  /**
   * @brief Get the error string.
   * @param status The status to get the error string.
   */
  std::string GetErrorString(const int32_t status);

 private:
  int is_started_;
  NTCAN_HANDLE dev_handler_;
  int port_;
  int baud_;
  CMSG send_frames_[MAX_CAN_SEND_FRAME_LEN];
  CMSG recv_frames_[MAX_CAN_RECV_FRAME_LEN];
};

#endif  // MODULES_DRIVERS_CANBUS_CAN_CLIENT_CLIENT_ESD_CAN_CLIENT_H_
