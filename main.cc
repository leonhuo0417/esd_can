#include <unistd.h>
#include "six_wheel_chassis.h"


int main() {
  EsdCanClient esd_can(0, NTCAN_BAUD_125);
  esd_can.Start();
  

  SixWheelChassis chassis(&esd_can);

  // CanFrame can_frame_id1;
  // CanFrame can_frame_id2;
  // CanFrame can_frame_id3;




  // can_frame_id1.id  = 0x20001314;
  // can_frame_id2.id  = 0x20001314;
  // can_frame_id3.id  = 0x20001314;

  // can_frame_id1.len = 8;
  // can_frame_id2.len = 8;
  // can_frame_id3.len = 3;

  // can_frame_id1.data[0] = 0x0aa;
  // can_frame_id1.data[1] = 0x55;
  // can_frame_id1.data[2] = 0x0f;
  // can_frame_id1.data[3] = 0x12;
  // can_frame_id1.data[4] = 0x01;
  // can_frame_id1.data[5] = 0x00;
  // can_frame_id1.data[6] = 0x0cd;
  // can_frame_id1.data[7] = 0x0cc;

  // can_frame_id2.data[0] = 0x0cc;
  // can_frame_id2.data[1] = 0x3e;
  // can_frame_id2.data[2] = 0x00;
  // can_frame_id2.data[3] = 0x00;
  // can_frame_id2.data[4] = 0x00;
  // can_frame_id2.data[5] = 0x00;
  // can_frame_id2.data[6] = 0x00;
  // can_frame_id2.data[7] = 0x00;

  // can_frame_id3.data[0] = 0x00;
  // can_frame_id3.data[1] = 0x00;
  // can_frame_id3.data[2] = 0x10;


  // std::vector<CanFrame> vec_can;
  // vec_can.push_back(can_frame_id1);
  // vec_can.push_back(can_frame_id2);
  // vec_can.push_back(can_frame_id3);

  // int32_t frame_num = 3; //MAX_CAN_SEND_FRAME_LEN;
  float vel, angle;
  
  while (true)
  {
    // esd_can.Send(vec_can, &frame_num);
    chassis.sendData(1.0, 0);
    usleep(1000 * 30); // 40hz
    //chassis.receiveData(vel, angle);
  }

  return 0;
}