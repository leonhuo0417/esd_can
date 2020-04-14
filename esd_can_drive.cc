// Copyright (c) 2019 Fujian(Quanzhou)-HIT Research Institute of Engineering and Technology All rights reserved.

#include "esd_can_drive.h"

EsdCanClient::~EsdCanClient() {
  if (dev_handler_) {
    Stop();
  }
}

int EsdCanClient::Start() {

  uint32_t mode = 0;
  //mode |= NTCAN_MODE_NO_RTR;
  if (port_ > MAX_CAN_PORT || port_ < 0) {
    std::cout << "can port number [" << port_ << "] is out of the range [0,"
              << MAX_CAN_PORT << "]" << std::endl;
    return -1;
  }

  int32_t ret = canOpen(port_, mode, NTCAN_MAX_TX_QUEUESIZE,
                        NTCAN_MAX_RX_QUEUESIZE, 5, 5, &dev_handler_);
  if (ret != NTCAN_SUCCESS) {  
    std::cout << "open device error code [" << ret << "]: " << GetErrorString(ret) << std::endl;
    return -1;
  }

  // init config and state
  // After a CAN handle is created with canOpen() the CAN-ID filter is
  // cleared
  // (no CAN messages
  // will pass the filter). To receive a CAN message with a certain CAN-ID
  // or an
  // NTCAN-Event with
  // a certain Event-ID it is required to enable this ID in the handle
  // filter as
  // otherwise a
  // received  message or event is discarded by the driver for this handle.
  // 1. set receive message_id filter, ie white list
  int32_t id_count = 0x20000000; //0x1FFFFFFF;
  // ret = canIdRegionAdd(dev_handler_, 0x20000000, &id_count);
  ret = canIdAdd(dev_handler_, 0x20000000);
  if (ret != NTCAN_SUCCESS) {
    std::cout << "add receive msg id filter error code: " << ret << ", "
              << GetErrorString(ret) << std::endl;
    return -1;
  }

  // 2. set baudrate to 500k
  ret = canSetBaudrate(dev_handler_, baud_);
  if (ret != NTCAN_SUCCESS) {
    std::cout << "set baudrate error code: " << ret << ", " << GetErrorString(ret) << std::endl;
    return -1;
  }

  is_started_ = true;

  return 1;
}

void EsdCanClient::Stop() {
  if (is_started_) {
    is_started_ = false;
    int32_t ret = canClose(dev_handler_);
    if (ret != NTCAN_SUCCESS) {
      std::cout << "close error code:" << ret << ", " << GetErrorString(ret) << std::endl;
    } else {
      std::cout << "close esd can ok. port:" << port_ << std::endl;
    }
  }
}

// Synchronous transmission of CAN messages
int EsdCanClient::Send(const std::vector<CanFrame> &frames,
                             int32_t *const frame_num) {
  if (!is_started_) {
    std::cout << "Esd can client has not been initiated! Please init first!" << std::endl;
    return -1;
  }
  for (size_t i = 0; i < frames.size() && i < MAX_CAN_SEND_FRAME_LEN; ++i) {
    send_frames_[i].id = frames[i].id;
    send_frames_[i].len = frames[i].len;
    memcpy(send_frames_[i].data, frames[i].data, frames[i].len);
  }

  // Synchronous transmission of CAN messages
  int32_t ret = canWrite(dev_handler_, send_frames_, frame_num, nullptr);
  if (ret != NTCAN_SUCCESS) {
    std::cout << "send message failed, error code: " << ret << ", "
           << GetErrorString(ret) << std::endl;
    return -1;
  }
  return 1;
}

// buf size must be 8 bytes, every time, we receive only one frame
int EsdCanClient::Receive(std::vector<CanFrame> *const frames,
                                int32_t *const frame_num) {
  if (!is_started_) {
    std::cout << "Esd can client is not init! Please init first!" << std::endl;
    return -1;
  }

  if (*frame_num > MAX_CAN_RECV_FRAME_LEN || *frame_num < 0) {
    std::cout << "recv can frame num not in range[0, " << MAX_CAN_RECV_FRAME_LEN
           << "], frame_num:" << *frame_num << std::endl;
    // TODO(Authors): check the difference of returning frame_num/error_code
    return -1;
  }

  const int32_t ret = canRead(dev_handler_, recv_frames_, frame_num, nullptr);
  // rx timeout not log
  if (ret == NTCAN_RX_TIMEOUT) {
    return 1;
  }
  if (ret != NTCAN_SUCCESS) {
    std::cout << "receive message failed, error code: " << ret << ", "
           << GetErrorString(ret) << std::endl;
    return -1;
  }

  for (int32_t i = 0; i < *frame_num && i < MAX_CAN_RECV_FRAME_LEN; ++i) {
    CanFrame cf;
    cf.id = recv_frames_[i].id;
    cf.len = recv_frames_[i].len;
    memcpy(cf.data, recv_frames_[i].data, recv_frames_[i].len);
    frames->push_back(cf);
  }

  return 1;
}

/************************************************************************/
/************************************************************************/
/* Function: GetErrorString()                                            */
/* Return ASCII representation of NTCAN return code                     */
/************************************************************************/
/************************************************************************/
const int32_t ERROR_BUF_SIZE = 200;
std::string EsdCanClient::GetErrorString(const NTCAN_RESULT ntstatus) {
  struct ERR2STR {
    NTCAN_RESULT ntstatus;
    const char *str;
  };

  int8_t str_buf[ERROR_BUF_SIZE];

  static const struct ERR2STR err2str[] = {
      {NTCAN_SUCCESS, "NTCAN_SUCCESS"},
      {NTCAN_RX_TIMEOUT, "NTCAN_RX_TIMEOUT"},
      {NTCAN_TX_TIMEOUT, "NTCAN_TX_TIMEOUT"},
      {NTCAN_TX_ERROR, "NTCAN_TX_ERROR"},
      {NTCAN_CONTR_OFF_BUS, "NTCAN_CONTR_OFF_BUS"},
      {NTCAN_CONTR_BUSY, "NTCAN_CONTR_BUSY"},
      {NTCAN_CONTR_WARN, "NTCAN_CONTR_WARN"},
      {NTCAN_NO_ID_ENABLED, "NTCAN_NO_ID_ENABLED"},
      {NTCAN_ID_ALREADY_ENABLED, "NTCAN_ID_ALREADY_ENABLED"},
      {NTCAN_ID_NOT_ENABLED, "NTCAN_ID_NOT_ENABLED"},
      {NTCAN_INVALID_FIRMWARE, "NTCAN_INVALID_FIRMWARE"},
      {NTCAN_MESSAGE_LOST, "NTCAN_MESSAGE_LOST"},
      {NTCAN_INVALID_PARAMETER, "NTCAN_INVALID_PARAMETER"},
      {NTCAN_INVALID_HANDLE, "NTCAN_INVALID_HANDLE"},
      {NTCAN_NET_NOT_FOUND, "NTCAN_NET_NOT_FOUND"},
#ifdef NTCAN_IO_INCOMPLETE
      {NTCAN_IO_INCOMPLETE, "NTCAN_IO_INCOMPLETE"},
#endif
#ifdef NTCAN_IO_PENDING
      {NTCAN_IO_PENDING, "NTCAN_IO_PENDING"},
#endif
#ifdef NTCAN_INVALID_HARDWARE
      {NTCAN_INVALID_HARDWARE, "NTCAN_INVALID_HARDWARE"},
#endif
#ifdef NTCAN_PENDING_WRITE
      {NTCAN_PENDING_WRITE, "NTCAN_PENDING_WRITE"},
#endif
#ifdef NTCAN_PENDING_READ
      {NTCAN_PENDING_READ, "NTCAN_PENDING_READ"},
#endif
#ifdef NTCAN_INVALID_DRIVER
      {NTCAN_INVALID_DRIVER, "NTCAN_INVALID_DRIVER"},
#endif
#ifdef NTCAN_OPERATION_ABORTED
      {NTCAN_OPERATION_ABORTED, "NTCAN_OPERATION_ABORTED"},
#endif
#ifdef NTCAN_WRONG_DEVICE_STATE
      {NTCAN_WRONG_DEVICE_STATE, "NTCAN_WRONG_DEVICE_STATE"},
#endif
      {NTCAN_INSUFFICIENT_RESOURCES, "NTCAN_INSUFFICIENT_RESOURCES"},
#ifdef NTCAN_HANDLE_FORCED_CLOSE
      {NTCAN_HANDLE_FORCED_CLOSE, "NTCAN_HANDLE_FORCED_CLOSE"},
#endif
#ifdef NTCAN_NOT_IMPLEMENTED
      {NTCAN_NOT_IMPLEMENTED, "NTCAN_NOT_IMPLEMENTED"},
#endif
#ifdef NTCAN_NOT_SUPPORTED
      {NTCAN_NOT_SUPPORTED, "NTCAN_NOT_SUPPORTED"},
#endif
#ifdef NTCAN_SOCK_CONN_TIMEOUT
      {NTCAN_SOCK_CONN_TIMEOUT, "NTCAN_SOCK_CONN_TIMEOUT"},
#endif
#ifdef NTCAN_SOCK_CMD_TIMEOUT
      {NTCAN_SOCK_CMD_TIMEOUT, "NTCAN_SOCK_CMD_TIMEOUT"},
#endif
#ifdef NTCAN_SOCK_HOST_NOT_FOUND
      {NTCAN_SOCK_HOST_NOT_FOUND, "NTCAN_SOCK_HOST_NOT_FOUND"},
#endif
#ifdef NTCAN_CONTR_ERR_PASSIVE
      {NTCAN_CONTR_ERR_PASSIVE, "NTCAN_CONTR_ERR_PASSIVE"},
#endif
#ifdef NTCAN_ERROR_NO_BAUDRATE
      {NTCAN_ERROR_NO_BAUDRATE, "NTCAN_ERROR_NO_BAUDRATE"},
#endif
#ifdef NTCAN_ERROR_LOM
      {NTCAN_ERROR_LOM, "NTCAN_ERROR_LOM"},
#endif
      {(NTCAN_RESULT)0xffffffff, "NTCAN_UNKNOWN"} /* stop-mark */
  };

  const struct ERR2STR *es = err2str;

  do {
    if (es->ntstatus == ntstatus) {
      break;
    }
    es++;
  } while ((uint32_t)es->ntstatus != 0xffffffff);

#ifdef NTCAN_ERROR_FORMAT_LONG
  {
    NTCAN_RESULT res;
    char sz_error_text[60];

    res = canFormatError(ntstatus, NTCAN_ERROR_FORMAT_LONG, sz_error_text,
                         sizeof(sz_error_text) - 1);
    if (NTCAN_SUCCESS == res) {
      snprintf(reinterpret_cast<char *>(str_buf), ERROR_BUF_SIZE, "%s - %s",
               es->str, sz_error_text);
    } else {
      snprintf(reinterpret_cast<char *>(str_buf), ERROR_BUF_SIZE, "%s(0x%08x)",
               es->str, ntstatus);
    }
  }
#else
  snprintf(reinterpret_cast<char *>(str_buf), ERROR_BUF_SIZE, "%s(0x%08x)",
           es->str, ntstatus);
#endif /* of NTCAN_ERROR_FORMAT_LONG */

  return std::string((const char *)(str_buf));
}