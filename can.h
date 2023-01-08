// TODO - more abstractions for various components...
// See https://github.com/pierremolinaro/acan2515Tiny/tree/master/examples 
void can_setup();
void can_loop();

/* SimpBMS payloads

  msg.id  = 0x351;
  msg.len = 8;
  if (storagemode == 0)
  {
    msg.buf[0] = lowByte(uint16_t((settings.ChargeVsetpoint * settings.Scells ) * 10));
    msg.buf[1] = highByte(uint16_t((settings.ChargeVsetpoint * settings.Scells ) * 10));
  }
  else
  {
    msg.buf[0] = lowByte(uint16_t((settings.StoreVsetpoint * settings.Scells ) * 10));
    msg.buf[1] = highByte(uint16_t((settings.StoreVsetpoint * settings.Scells ) * 10));
  }
  msg.buf[2] = lowByte(chargecurrent);
  msg.buf[3] = highByte(chargecurrent);
  msg.buf[4] = lowByte(discurrent );
  msg.buf[5] = highByte(discurrent);
  msg.buf[6] = lowByte(uint16_t((settings.DischVsetpoint * settings.Scells) * 10));
  msg.buf[7] = highByte(uint16_t((settings.DischVsetpoint * settings.Scells) * 10));
  Can0.write(msg);

  msg.id  = 0x355;
  msg.len = 8;
  msg.buf[0] = lowByte(SOC);
  msg.buf[1] = highByte(SOC);
  msg.buf[2] = lowByte(SOH);
  msg.buf[3] = highByte(SOH);
  msg.buf[4] = lowByte(SOC * 10);
  msg.buf[5] = highByte(SOC * 10);
  msg.buf[6] = 0;
  msg.buf[7] = 0;
  Can0.write(msg);

  msg.id  = 0x356;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(bms.getPackVoltage() * 100));
  msg.buf[1] = highByte(uint16_t(bms.getPackVoltage() * 100));
  msg.buf[2] = lowByte(long(currentact / 100));
  msg.buf[3] = highByte(long(currentact / 100));
  msg.buf[4] = lowByte(int16_t(bms.getAvgTemperature() * 10));
  msg.buf[5] = highByte(int16_t(bms.getAvgTemperature() * 10));
  msg.buf[6] = 0;
  msg.buf[7] = 0;
  Can0.write(msg);

  delay(2);
  msg.id  = 0x35A;
  msg.len = 8;
  msg.buf[0] = alarm[0];//High temp  Low Voltage | High Voltage
  msg.buf[1] = alarm[1]; // High Discharge Current | Low Temperature
  msg.buf[2] = alarm[2]; //Internal Failure | High Charge current
  msg.buf[3] = alarm[3];// Cell Imbalance
  msg.buf[4] = warning[0];//High temp  Low Voltage | High Voltage
  msg.buf[5] = warning[1];// High Discharge Current | Low Temperature
  msg.buf[6] = warning[2];//Internal Failure | High Charge current
  msg.buf[7] = warning[3];// Cell Imbalance
  Can0.write(msg);

  msg.id  = 0x35E;
  msg.len = 8;
  msg.buf[0] = bmsname[0];
  msg.buf[1] = bmsname[1];
  msg.buf[2] = bmsname[2];
  msg.buf[3] = bmsname[3];
  msg.buf[4] = bmsname[4];
  msg.buf[5] = bmsname[5];
  msg.buf[6] = bmsname[6];
  msg.buf[7] = bmsname[7];
  Can0.write(msg);

  delay(2);
  msg.id  = 0x370;
  msg.len = 8;
  msg.buf[0] = bmsmanu[0];
  msg.buf[1] = bmsmanu[1];
  msg.buf[2] = bmsmanu[2];
  msg.buf[3] = bmsmanu[3];
  msg.buf[4] = bmsmanu[4];
  msg.buf[5] = bmsmanu[5];
  msg.buf[6] = bmsmanu[6];
  msg.buf[7] = bmsmanu[7];
  Can0.write(msg);

  if (balancecells == 1)
  {
    if (bms.getLowCellVolt() + settings.balanceHyst < bms.getHighCellVolt())
    {
      msg.id  = 0x3c3;
      msg.len = 8;
      if (bms.getLowCellVolt() < settings.balanceVoltage)
      {
        msg.buf[0] = highByte(uint16_t(settings.balanceVoltage * 1000));
        msg.buf[1] = lowByte(uint16_t(settings.balanceVoltage * 1000));
      }
      else
      {
        msg.buf[0] = highByte(uint16_t(bms.getLowCellVolt() * 1000));
        msg.buf[1] = lowByte(uint16_t(bms.getLowCellVolt() * 1000));
      }
      msg.buf[2] =  0x01;
      msg.buf[3] =  0x04;
      msg.buf[4] =  0x03;
      msg.buf[5] =  0x00;
      msg.buf[6] =  0x00;
      msg.buf[7] = 0x00;
      Can0.write(msg);
    }
  }


*/


/*
Tesla Charger CAN messages

It may be set up to automatically send messages to DC/DC to get it up to 14V (default failsafe is 13.5v)

int StatusID = 0x410;

  uint16_t y, z = 0;
  outframe.id = StatusID;
  if (parameters.canControl == 3)
  {
    outframe.id = StatusID + 1;
  }
  outframe.length = 8;            // Data payload 8 bytes
  outframe.extended = 0;          // Extended addresses - 0=11-bit 1=29bit
  outframe.rtr = 0;                 //No request
  outframe.data.bytes[0] = 0x00;
  for (int x = 0; x < 3; x++)
  {
    y = y +  dcvolt[x] ;
  }
  outframe.data.bytes[0] = y / 3;

  if (parameters.phaseconfig == Singlephase)
  {
    for (int x = 0; x < 3; x++)
    {
      z = z + (accur[x] * 66.66) ;
    }
  }
  else
  {
    z = accur[2] * 66.66;
  }

  outframe.data.bytes[1] = lowByte (z);
  outframe.data.bytes[2] = highByte (z);

  outframe.data.bytes[3] = lowByte (uint16_t (totdccur)); //0.005Amp
  outframe.data.bytes[4] = highByte (uint16_t (totdccur));  //0.005Amp
  outframe.data.bytes[5] = lowByte (uint16_t (modulelimcur * 0.66666));
  outframe.data.bytes[6] = highByte (uint16_t (modulelimcur * 0.66666));
  outframe.data.bytes[7] = 0x00;
  outframe.data.bytes[7] = Proximity << 6;
  outframe.data.bytes[7] = outframe.data.bytes[7] || (parameters.type << 4);
  Can1.sendFrame(outframe);


  outframe.id = ElconID;
  if ( parameters.canControl == 3)
  {
    outframe.id = ElconID + 1;
  }
  outframe.id = ElconID;
  outframe.length = 8;            // Data payload 8 bytes
  outframe.extended = 1;          // Extended addresses - 0=11-bit 1=29bit
  outframe.rtr = 0;                 //No request


  outframe.data.bytes[0] = highByte (y * 10 / 3);
  outframe.data.bytes[1] = lowByte (y * 10 / 3);
  outframe.data.bytes[2] = highByte (uint16_t (totdccur * 20)); //0.005Amp conv to 0.1
  outframe.data.bytes[3] = lowByte (uint16_t (totdccur * 20)); //0.005Amp conv to 0.1
  outframe.data.bytes[4] = 0x00;
  outframe.data.bytes[5] = 0x00;
  outframe.data.bytes[6] = 0x00;
  outframe.data.bytes[7] = 0x00;
  Can1.sendFrame(outframe);

  ///DCDC CAN//////////////////////////////////////////////////////////////////////
  if (dcdcenable)
  {
    outframe.id = 0x3D8;
    outframe.length = 3;            // Data payload 8 bytes
    outframe.extended = 0;          // Extended addresses - 0=11-bit 1=29bit
    outframe.rtr = 0;                 //No request

    outframe.data.bytes[0] = highByte (uint16_t((parameters.dcdcsetpoint - 9000) / 68.359375) << 6);
    outframe.data.bytes[1] = lowByte (uint16_t((parameters.dcdcsetpoint - 9000) / 68.359375) << 6);

    outframe.data.bytes[1] = outframe.data.bytes[1] | 0x20;
    outframe.data.bytes[2] = 0x00;
    Can1.sendFrame(outframe);
  }


  if (parameters.canControl == 1)
  {
    outframe.id = ControlID;
    outframe.length = 8;            // Data payload 8 bytes
    outframe.extended = 0;          // Extended addresses - 0=11-bit 1=29bit
    outframe.rtr = 0;                 //No request

    outframe.data.bytes[0] = 0;

    if (state != 0)
    {
      if ( slavechargerenable == 1)
      {
        outframe.data.bytes[0] = 0x01;
      }
    }

    outframe.data.bytes[1] = highByte(parameters.voltSet);
    outframe.data.bytes[2] = lowByte(parameters.voltSet);
    outframe.data.bytes[3] = highByte(maxdccur);
    outframe.data.bytes[4] = lowByte(maxdccur);
    outframe.data.bytes[5] = highByte(modulelimcur);
    outframe.data.bytes[6] = lowByte(modulelimcur);
    outframe.data.bytes[7] = 0;

    Can1.sendFrame(outframe);
  }
*/

/*

Tesla DC/DC

https://openinverter.org/wiki/Tesla_Model_S/X_DC/DC_Converter

Reporting does not work if enable line is not high. Whenever i turn on enable i see reports.

Report is in telegram 0x210

0x210 8 bytes at 100ms
byte 2 = coolant inlet temp,
byte 3 = input power (16 W/bit),
byte 4 = output current (1 A/bit),
byte 5 = output voltage (0.1 V/bit)


*/


/*
Tesla LDU / OpenInverter 

https://openinverter.org/wiki/CAN_table_CAN_STD
Shows some "standard" messages, maybe these will trickle out without needing to request data.  These all have CAN ID's of 0x3FF 
If not...

Looks like it requires sending a request to get a response

https://openinverter.org/wiki/CAN_communication#Setting_and_reading_parameters_via_SDO

https://openinverter.org/wiki/Parameters

Req: 0x601
resp: 0x581

The value index must be determined by counting the output of the list command. E.g. "boost" at the very top has index 0, potnom has index 77

VALUE_ENTRY(speed,       "rpm",   2012 ) \
param_prj.h

CAN ID # Byte 1 (Cmd)	Bytes 2-3 (Index)	Byte 4 (Subindex)	Bytes 5-8 (Data)
0x601 # 0x40 0x00 0x20 0x00 0 0 0 0 Get value of "boost"

0x601 # 0x40 0x10 0x20 0x0D 0 0 0 0 Get value of "pwmfrq" on any firmware version or build (0x0D = 13 from the PARAM_ENTRY() for "pwmfrq" in param_prj.h)

Other potential values of interesting...

    VALUE_ENTRY(turns,       "",      2037 ) \
      Number of turns the motor completed since power up

    VALUE_ENTRY(opmode,      OPMODES, 2000 ) \
      Operating mode. 0=Off, 1=Run, 2=Manual_run, 3=Boost, 4=Buck, 5=Sine, 6=2 Phase sine
    VALUE_ENTRY(lasterr,     errorListString,  2038 ) \
    VALUE_ENTRY(status,      STATUS,  2044 ) \
      #define STATUS       "0=None, 1=UdcLow, 2=UdcHigh, 4=UdcBelowUdcSw, 8=UdcLim, 16=EmcyStop, 32=MProt, 64=PotPressed, 128=TmpHs, 256=WaitStart"

    VALUE_ENTRY(tmphs,       "°C",    2019 ) \ <-- Heat Sink (invernter)
    VALUE_ENTRY(tmpm,        "°C",    2020 ) \ <-- Motor
    VALUE_ENTRY(idc,         "A",     2002 ) \
      Calculated DC current: positive run, negative regen
    VALUE_ENTRY(uaux,        "V",     2021 ) \
      Auxiliary voltage (i.e. 12V system)

*/