// Copyright (c) 2019 Fujian(Quanzhou)-HIT Research Institute of Engineering and Technology All rights reserved.

#ifndef SIX_WHEEL_CHASSIS_H
#define SIX_WHEEL_CHASSIS_H

#include "esd_can_drive.h"
#include <iomanip>

#define HEAD_FRAME_HIGH 0x0aa
#define HEAD_FRAME_LOW  0x55
#define VALID_LENGTH    0x0f
#define LARGE_FLAG      0x12
#define SMALL_FLAG      0x01
#define BREAK_OR_MODE   0x00

#define FRAME_LENGTH_1ST 8
#define FRAME_LENGTH_2ND 8
#define FRAME_LENGTH_3RD 3

#define REV_FRAME_LENGTH 54

#define EXTEND_CAN_FRAME_ID 0x20001314

class SixWheelChassis
{
private:
  EsdCanClient* esd_can_;
public:
  SixWheelChassis();
  SixWheelChassis(EsdCanClient* esd_can) : esd_can_(esd_can) {}
  // {
  //   // esd_can_->Start();
  // }
  ~SixWheelChassis() {}

  uint8_t xorCheck(uint8_t* data, int len);
  void sendData(float vel, float angle);
  void receiveData(float &vel, float &angle);
};

union ExpData
{
  float fdata;
  char sdata[4];
}exp_data;

uint8_t SixWheelChassis::xorCheck(uint8_t* data, int len)
{
  uint8_t sum = 0;

  for (int i = 0; i < len; ++i)
  {
    sum ^= data[i];
  }

  return sum;
}

void SixWheelChassis::sendData(float vel, float angle)
{
  CanFrame can_frame_1st;
  CanFrame can_frame_2nd;
  CanFrame can_frame_3rd;

  can_frame_1st.id = EXTEND_CAN_FRAME_ID;
  can_frame_2nd.id = EXTEND_CAN_FRAME_ID;
  can_frame_3rd.id = EXTEND_CAN_FRAME_ID;

  can_frame_1st.len = FRAME_LENGTH_1ST;
  can_frame_2nd.len = FRAME_LENGTH_2ND;
  can_frame_3rd.len = FRAME_LENGTH_3RD;

  const int LENGTH = FRAME_LENGTH_1ST + FRAME_LENGTH_2ND + FRAME_LENGTH_3RD;

  uint8_t data[LENGTH];

  exp_data.fdata = vel;

  can_frame_1st.data[0] = HEAD_FRAME_HIGH;
  can_frame_1st.data[1] = HEAD_FRAME_LOW;
  can_frame_1st.data[2] = VALID_LENGTH;
  can_frame_1st.data[3] = LARGE_FLAG;
  can_frame_1st.data[4] = SMALL_FLAG;
  can_frame_1st.data[5] = BREAK_OR_MODE;
  can_frame_1st.data[6] = exp_data.sdata[0];
  can_frame_1st.data[7] = exp_data.sdata[1];

  can_frame_2nd.data[0] = exp_data.sdata[2];
  can_frame_2nd.data[1] = exp_data.sdata[3];
  can_frame_2nd.data[2] = 0x00;
  can_frame_2nd.data[3] = 0x00;
  can_frame_2nd.data[4] = 0x00;
  can_frame_2nd.data[5] = 0x00;

  exp_data.fdata = angle;

  can_frame_2nd.data[6] = exp_data.sdata[0];
  can_frame_2nd.data[7] = exp_data.sdata[1];

  can_frame_3rd.data[0] = exp_data.sdata[2];
  can_frame_3rd.data[1] = exp_data.sdata[3];

  memcpy(data, can_frame_1st.data, FRAME_LENGTH_1ST);
  memcpy(data + 8, can_frame_2nd.data, FRAME_LENGTH_2ND);
  memcpy(data + 16, can_frame_3rd.data, FRAME_LENGTH_3RD);

  uint8_t sum = xorCheck(data, LENGTH - 1);
  can_frame_3rd.data[2] = sum;

  std::vector<CanFrame> vec_can;
  vec_can.push_back(can_frame_1st);
  vec_can.push_back(can_frame_2nd);
  vec_can.push_back(can_frame_3rd);

  int32_t frame_num = MAX_CAN_SEND_FRAME_LEN;

  esd_can_->Send(vec_can, &frame_num);
}

void SixWheelChassis::receiveData(float &vel, float &angle)
{
  std::vector<CanFrame> buf;
  int32_t len = MAX_CAN_RECV_FRAME_LEN;

  int ret = esd_can_->Receive(&buf, &len);

  // for (int i = 0; i < buf[0].len; ++i)
  // {
  //   std::cout << std::hex << int(buf[0].data[i]) << " ";
  // }
  // std::cout << std::endl;
  uint8_t data[REV_FRAME_LENGTH];
  for (int i = 0; i < buf.size(); ++i)
  {
    for (int j = 0; j < buf[i].len; ++j)
    {
      data[i * buf[i].len + j] = buf[i].data[j];
      // std::cout << std::hex << int(buf[i].data[j]) << " ";
    }
    //std::cout << std::endl;
  }
  //int a = buf[0].data[0];  
  int sum = 0, avg = 0;
  int vel_loc = 3, vel_end = 16;
  if (HEAD_FRAME_HIGH == data[0] && HEAD_FRAME_LOW == data[1])
  {
    for (int i = vel_loc; i < vel_end; i += 2)
    {
      sum += (int)data[i] + (int)data[i + 1] << 8;
    }
    
    avg = sum / 6;
    std::cout << "avg = " << avg << std::endl;
  }
}

#endif