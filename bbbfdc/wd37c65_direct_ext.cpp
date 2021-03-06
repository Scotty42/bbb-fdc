#include <python2.7/Python.h>
#include <unistd.h>
#include <stdio.h>
#include <iobb.h>

#include "wiringPi.h"

#define RESULT_OKAY 0
#define RESULT_TIMEOUT_EXEC 0x14
#define RESULT_OVER_DRAIN 0x16
#define RESULT_OVER_CMDRES 0x17
#define RESULT_TIMEOUT_READRES 0x18
#define RESULT_READ_ERROR 0x19
#define RESULT_WRITE_ERROR 0x20
#define RESULT_SHORT 0x21
#define RESULT_LONG 0x22
#define RESULT_INPROCESS 0x23

#define REG_MSR 0
#define REG_DATA 1

//////////////////////////// DEFINE PINs /////////////////////////////

#define CS_FDC 0
#define CS_DOR 1
#define CS_DCR 2

#define WD_WR     BBBIO_GPIO_PIN_31   //  BBBIO_GPIO0
#define WD_RD     BBBIO_GPIO_PIN_30   //  BBBIO_GPIO0
#define WD_A0     BBBIO_GPIO_PIN_22   //  BBBIO_GPIO0
#define WD_CS     BBBIO_GPIO_PIN_2    //  BBBIO_GPIO0
#define WD_DOR    BBBIO_GPIO_PIN_3    //  BBBIO_GPIO0
#define WD_CCR    BBBIO_GPIO_PIN_4    //  BBBIO_GPIO0
#define WD_DACK   BBBIO_GPIO_PIN_5    //  BBBIO_GPIO0
#define WD_TC     BBBIO_GPIO_PIN_23   //  BBBIO_GPIO0
#define WD_RESET  BBBIO_GPIO_PIN_1    //  BBBIO_GPIO2
#define WD_DC     BBBIO_GPIO_PIN_7    //  BBBIO_GPIO0

const int WD_DATAPINS[] = {BBBIO_GPIO_PIN_12 , BBBIO_GPIO_PIN_13 , BBBIO_GPIO_PIN_14 , BBBIO_GPIO_PIN_15 , BBBIO_GPIO_PIN_16 , BBBIO_GPIO_PIN_17 , BBBIO_GPIO_PIN_18 , BBBIO_GPIO_PIN_19};  // BBBIO_GPIO1
const int WD_DATAPINS_REVERSED[]  = {BBBIO_GPIO_PIN_19 , BBBIO_GPIO_PIN_18 , BBBIO_GPIO_PIN_17 , BBBIO_GPIO_PIN_16 , BBBIO_GPIO_PIN_15 , BBBIO_GPIO_PIN_14 , BBBIO_GPIO_PIN_13 , BBBIO_GPIO_PIN_12};   // BBBIO_GPIO1

//////////////////////////// DEFINE PINs /////////////////////////////

void check_root_user(void)
{
    if(geteuid()!=0)
        fprintf(stderr, "Run as root user! (or use sudo)\n");
}

void myDelayMicroseconds(int x)
{
  // use pseudo wiringPi adapted to BBB
  delayMicroseconds(x);
}

void wd_deinit(void) 
{
  BBBIO_sys_Disable_GPIO(BBBIO_GPIO0);
  BBBIO_sys_Disable_GPIO(BBBIO_GPIO1);
  BBBIO_sys_Disable_GPIO(BBBIO_GPIO2);

  iolib_free();
  fprintf(stdout, "GPIO DeInit\n");
}

void wd_init(void)
{
  check_root_user();
  iolib_init();
  fprintf(stdout, "GPIO Init\n");

  BBBIO_sys_Enable_GPIO(BBBIO_GPIO0);
  BBBIO_GPIO_set_dir(BBBIO_GPIO0, 
    BBBIO_GPIO_PIN_0 | BBBIO_GPIO_PIN_7,     // input
    BBBIO_GPIO_PIN_22 | BBBIO_GPIO_PIN_30 | BBBIO_GPIO_PIN_31 | BBBIO_GPIO_PIN_2 | BBBIO_GPIO_PIN_3 | BBBIO_GPIO_PIN_4 | BBBIO_GPIO_PIN_5 | BBBIO_GPIO_PIN_23 );   // output, default pulldown/pulldown in OCP mode 7

  BBBIO_GPIO_high(BBBIO_GPIO0, WD_A0);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_RD);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_WR);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_CS);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_DOR);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_CCR);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_DACK);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_TC);

  BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);
  BBBIO_GPIO_set_dir(BBBIO_GPIO2, 
    BBBIO_GPIO_PIN_0,     // input
    BBBIO_GPIO_PIN_1 );   // output, default pulldown/pulldown in OCP mode 7

  BBBIO_GPIO_low(BBBIO_GPIO2, WD_RESET);

  // data port pins
  // default OCP pinmux behavior in mode 7 := pulldown
  BBBIO_sys_Enable_GPIO(BBBIO_GPIO1);
  BBBIO_GPIO_set_dir(BBBIO_GPIO1, 
    BBBIO_GPIO_PIN_0, 
    BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_13 | BBBIO_GPIO_PIN_14 | BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16 | BBBIO_GPIO_PIN_17 | BBBIO_GPIO_PIN_18 | BBBIO_GPIO_PIN_19);

  // reset WDC
  fprintf(stdout, "WD reset\n");
  myDelayMicroseconds(10);
  BBBIO_GPIO_high(BBBIO_GPIO2, WD_RESET);  // assert reset
  myDelayMicroseconds(100);
  BBBIO_GPIO_low(BBBIO_GPIO2, WD_RESET);   // deassert reset
  myDelayMicroseconds(1000);

  fprintf(stdout, "GPIO Init done\n");
}

void wd_config_input(void)
{
  //BBBIO_sys_Disable_GPIO(BBBIO_GPIO1);

  BBBIO_GPIO_set_dir(BBBIO_GPIO1, 
    BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_13 | BBBIO_GPIO_PIN_14 | BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16 | BBBIO_GPIO_PIN_17 | BBBIO_GPIO_PIN_18 | BBBIO_GPIO_PIN_19,
    BBBIO_GPIO_PIN_0);
  //fprintf(stdout, "wd_config_input\n");
}

void wd_config_output(void)
{
  //BBBIO_sys_Disable_GPIO(BBBIO_GPIO1);

  BBBIO_GPIO_set_dir(BBBIO_GPIO1, 
    BBBIO_GPIO_PIN_0, 
    BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_13 | BBBIO_GPIO_PIN_14 | BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16 | BBBIO_GPIO_PIN_17 | BBBIO_GPIO_PIN_18 | BBBIO_GPIO_PIN_19);
  //fprintf(stdout, "wd_config_output\n");
}

void wd_set_addr(unsigned int addr)
{
  addr == 1 ? BBBIO_GPIO_high(BBBIO_GPIO0, WD_A0) : BBBIO_GPIO_low(BBBIO_GPIO0, WD_A0);
}

void wd_set_rd(unsigned int value)
{
  value == 1 ? BBBIO_GPIO_high(BBBIO_GPIO0, WD_RD) : BBBIO_GPIO_low(BBBIO_GPIO0, WD_RD);
}

void wd_set_wr(unsigned int value)
{
  value == 1 ? BBBIO_GPIO_high(BBBIO_GPIO0, WD_WR) : BBBIO_GPIO_low(BBBIO_GPIO0, WD_WR);
}

void wd_set_cs(unsigned int cs, unsigned int value)
{
  switch(cs) {
    case CS_FDC:
      value == 1 ? BBBIO_GPIO_high(BBBIO_GPIO0, WD_CS) : BBBIO_GPIO_low(BBBIO_GPIO0, WD_CS);
      break;

    case CS_DOR:
      value == 1 ? BBBIO_GPIO_high(BBBIO_GPIO0, WD_DOR): BBBIO_GPIO_low(BBBIO_GPIO0, WD_DOR);
      break;

    case CS_DCR:
      value == 1 ? BBBIO_GPIO_high(BBBIO_GPIO0, WD_CCR) : BBBIO_GPIO_low(BBBIO_GPIO0, WD_CCR);
      break;
  }
}

void wd_set_data(unsigned int data)
{
  for (int n=0; n<8; n++) {
    (data & 0x01) == 1 ? BBBIO_GPIO_high(BBBIO_GPIO1, WD_DATAPINS[n]) : BBBIO_GPIO_low(BBBIO_GPIO1, WD_DATAPINS[n]);
    data = data >> 1;
  }
}

unsigned int wd_get_data(void)
{
  unsigned int data = 0;
  for (int n = 7; n >= 0; n--) {
    int bit = BBBIO_GPIO_get(BBBIO_GPIO1, WD_DATAPINS[n]) ? 1 : 0;
    data = (data << 1) + bit;
  }

  return data;
}

unsigned int wd_get_tc(void)
{
  return BBBIO_GPIO_get(BBBIO_GPIO0, WD_TC);  // XXX this is an output not an input ???
}

unsigned int wd_get_dc(void)
{
  return BBBIO_GPIO_get(BBBIO_GPIO0, WD_DC);
}

void wd_pulse_cs_wr(unsigned int cs)
{
  unsigned int i;

  wd_set_cs(cs, 0);
  wd_set_wr(0);

  // it's all fun and games until somebody loses an edge...
  for (i=0; i<1000; i++) {
    asm("nop");
  }

  wd_set_wr(1);
  wd_set_cs(cs, 1);
}

unsigned int wd_pulse_cs_rd(unsigned int cs)
{
  unsigned int i, v;

  wd_set_cs(cs, 0);
  wd_set_rd(0);

  // it's all fun and games until somebody loses an edge...
  for (i=0; i<1000; i++) {
    asm("nop");
  }

  v = wd_get_data();

  wd_set_rd(1);
  wd_set_cs(cs, 1);

  return v;
}

unsigned int wd_read_data(void) 
{
  unsigned int data;

  wd_config_input();
  wd_set_addr(REG_DATA);
  data = wd_pulse_cs_rd(CS_FDC);

  return data;
}

unsigned int wd_read_msr(void) 
{
  unsigned int msr;

  wd_config_input();
  wd_set_addr(REG_MSR);
  msr = wd_pulse_cs_rd(CS_FDC);

  return msr;
}

unsigned int wd_write_data(unsigned int data)
{
  //fprintf(stderr, "write data %02X\n", data);
  wd_config_output();
  wd_set_addr(REG_DATA);
  wd_set_data(data);
  wd_pulse_cs_wr(CS_FDC);
  wd_config_input();

  return data;
}

unsigned int wd_write_dor(unsigned int data)
{
  //fprintf(stderr, "write dor %02X\n", data);
  wd_config_output();
  wd_set_addr(0);
  wd_set_data(data);
  wd_pulse_cs_wr(CS_DOR);
  wd_config_input();

  return data;
}

unsigned int wd_write_dcr(unsigned int data)
{
  //fprintf(stderr, "write dcr %02X\n", data);
  wd_config_output();
  wd_set_addr(0);
  wd_set_data(data);
  wd_pulse_cs_wr(CS_DCR);
  wd_config_input();

  return data;
}

unsigned int wd_wait_msr(unsigned int mask, unsigned int val)
{
  unsigned int msr;
  unsigned int timeout = 65535 * 3;

  wd_config_input();
  while (TRUE) {
      // wiringPi
      myDelayMicroseconds(3);

      msr = wd_read_msr();
      //fprintf(stdout, "waitmsr (msr & mask): 0%02X\n", msr);
      if ((msr & mask) == val) {
        return 0;
      }

      timeout--;
      if (timeout == 0) {
        fprintf(stderr, "waitmsr timedout with %02X\n", msr);
        return RESULT_TIMEOUT_EXEC;
      }
  }
}

unsigned int wd_drain(void)
{
  unsigned int msr;
  unsigned int max = 1024;

  wd_config_input();
  while (max > 0) {
      // wiringPi
      myDelayMicroseconds(10);

      msr = wd_read_msr();
      if ((msr & 0xC0) != 0xC0) {
        // no data
        return 0;
      }

      // there is a byte
      wd_read_data();
      max --;
  }

  // something weird must have happened
  return RESULT_OVER_DRAIN;
}

void wd_reset(unsigned int dor)
{
    int i;

    wd_write_dor(0);
    myDelayMicroseconds(17);
    wd_write_dor(dor);

    // 2.4ms delay    
    for (i=0; i<240; i++) {
      // wiringPi
      myDelayMicroseconds(10);
    }
}

void wd_pulse_dack(void)
{
  BBBIO_GPIO_low(BBBIO_GPIO0, WD_DACK);
  myDelayMicroseconds(1);
  BBBIO_GPIO_high(BBBIO_GPIO0, WD_DACK);
}

void short_delay(void)
{
    // Just do nothing for a while. This is to allow the RAM some time to do it's work.
    //
    int j;

    for (j=0; j<1; j++) {
        asm("nop");
    }
}

///////////////////////////////////////// Python Wrapper definitions /////////////////////////////////////////////

static PyObject *wd_direct_init(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  wd_init();
  return Py_BuildValue("");
}

static PyObject *wd_direct_deinit(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  wd_deinit();
  return Py_BuildValue("");
}

static PyObject *wd_direct_reset(PyObject *self, PyObject *args)
{
  unsigned int dor;
  if (!PyArg_ParseTuple(args, "i", &dor)) {
    return NULL;
  }
  wd_reset(dor);
  return Py_BuildValue("");
}

static PyObject *wd_direct_pulse_dack(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  wd_pulse_dack();
  return Py_BuildValue("");
}

static PyObject *wd_direct_get_tc(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  return Py_BuildValue("i", wd_get_tc());
}

static PyObject *wd_direct_get_dc(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  return Py_BuildValue("i", wd_get_dc());
}

static PyObject *wd_direct_set_addr(PyObject *self, PyObject *args)
{
  int addr;  
  if (!PyArg_ParseTuple(args, "i", &addr)) {
    return NULL;    
  }
  wd_set_addr(addr);
  return Py_BuildValue("");
}

static PyObject *wd_direct_read_byte(PyObject *self, PyObject *args)
{
  unsigned int cs, addr;
  unsigned int data;
  if (!PyArg_ParseTuple(args, "ii", &cs, &addr)) {
    return NULL;
  }
  data = wd_read_data();
  return Py_BuildValue("i", data);
}

static PyObject *wd_direct_write_byte(PyObject *self, PyObject *args)
{
  unsigned int cs, addr, data;
  if (!PyArg_ParseTuple(args, "iii", &cs, &addr, &data)) {
    return NULL;
  }
  wd_config_output();
  wd_set_addr(addr);
  wd_set_data(data);
  wd_pulse_cs_wr(cs);
  wd_config_input();
  return Py_BuildValue("");
}

static PyObject *wd_direct_write_data(PyObject *self, PyObject *args)
{
  unsigned int data;
  if (!PyArg_ParseTuple(args, "i", &data)) {
    return NULL;
  }
  wd_write_data(data);
  return Py_BuildValue("");
}

static PyObject *wd_direct_write_dor(PyObject *self, PyObject *args)
{
  unsigned int data;
  if (!PyArg_ParseTuple(args, "i", &data)) {
    return NULL;
  }
  wd_write_dor(data);
  return Py_BuildValue("");
}

static PyObject *wd_direct_write_dcr(PyObject *self, PyObject *args)
{
  unsigned int data;
  if (!PyArg_ParseTuple(args, "i", &data)) {
    return NULL;
  }
  wd_write_dcr(data);
  return Py_BuildValue("");
}

static PyObject *wd_direct_read_block(PyObject *self, PyObject *args)
{
  unsigned int count;
  char buf[1024];
  int i;

  if (!PyArg_ParseTuple(args, "i", &count)) {
    return NULL;
  }

  if (count > 1024) {
      // throw exception?
      return NULL;
  }

  for (i=0; i<count; i++) {
      unsigned int status = wd_wait_msr(0xFF, 0xF0);   // RQM=1, DIO=1, NDM=1, BUS=1
      unsigned int msr;
      if (status!=0) {
        msr = wd_read_msr();
        if (msr == 0xD0) {
            // d0 indicates fdc wants us to read a byte
            fprintf(stderr, "read_block aborting on index %d with read error\n", i);
            return Py_BuildValue("is#", RESULT_READ_ERROR, buf, count);
        } else {
            fprintf(stderr, "read_block aborting on index %d with msr %02X\n", i, msr);
            return Py_BuildValue("is#", status, buf, count);
        }
      }

      buf[i] = wd_read_data();
  }

  // Terminate the transfer
  wd_pulse_dack();

  return Py_BuildValue("is#", 0, buf, count);
}

static PyObject *wd_direct_read_result(PyObject *self, PyObject *args)
{
  unsigned int count;
  unsigned int maxTime = 10000;
  char buf[1024];

  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  count = 0;
  while (maxTime>0) {
      unsigned int msr;

      // wiringPi
      myDelayMicroseconds(10);      
      
      msr = wd_read_msr();
      // fprintf(stderr, "readRes msr %2X\n", msr);
      if ((msr & 0xF0) == 0xD0) {
          // RQM=1, DIO=1, BUSY=1 ... byte is ready to read
          buf[count] = wd_read_data();
          count++;
          maxTime = 10000;
      } else if ((msr & 0xF0) == 0x80) {
          // RQM=1, DIO=0, BUSY=0 ... fdc is waiting for next command ... we are done
          // fprintf(stderr, "readRes data %s\n", buf);
          return Py_BuildValue("is#", 0, buf, count);
      } else {
         maxTime--;
      }

      if (count>128) {
        return Py_BuildValue("is#", RESULT_OVER_CMDRES, buf, count);
      }
  }

  return Py_BuildValue("is#", RESULT_TIMEOUT_READRES, buf, count);
}

static PyObject *wd_direct_write_block(PyObject *self, PyObject *args)
{
  const char *buf;
  unsigned int buf_len, count;
  unsigned int autoTerminate;
  int i;

  if (!PyArg_ParseTuple(args, "s#ii", &buf, &buf_len, &count, &autoTerminate)) {
    return NULL;
  }

  for (i=0; i<count; i++) {
      unsigned int status = wd_wait_msr(0xFF, 0xB0); // RQM=1, DIO=0, NDM=1, BUS=1
      if (status!=0) {
        unsigned int msr = wd_read_msr();
        if (msr == 0xD0) {
            // d0 indicates fdc wants us to read a byte
            fprintf(stderr, "write_block aborting on index %d with write error\n", i);
            return Py_BuildValue("i", RESULT_WRITE_ERROR);
        } else {
            fprintf(stderr, "write_block aborting on index %d with msr %02X\n", i, msr);
            return Py_BuildValue("i", status);
        }
      }

      wd_write_data(*buf);
      buf++;
  }

  if (autoTerminate) {
      // Terminate the transfer
      wd_pulse_dack();
  }

  return Py_BuildValue("i", 0);
}

static PyObject *wd_direct_write_command(PyObject *self, PyObject *args)
{
  const char *buf;
  unsigned int buf_len, count;
  int i;

  if (!PyArg_ParseTuple(args, "s#i", &buf, &buf_len, &count)) {
    return NULL;
  }

  for (i=0; i<count; i++) {
      unsigned int status = wd_wait_msr(0xC0, 0x80);
      if (status!=0) {
        fprintf(stdout, "write_command status %2X on byte %d\n", status, i);
        wd_config_input();
        return Py_BuildValue("i", status);
      }

      wd_write_data(*buf);
      buf++;
  }
  wd_config_input();
  return Py_BuildValue("i", 0);
}

static PyObject *wd_direct_wait_msr(PyObject *self, PyObject *args)
{
  unsigned int mask, value;
  unsigned int status;
  if (!PyArg_ParseTuple(args, "ii", &mask, &value)) {
    return NULL;
  }
  status = wd_wait_msr(mask, value);
  return Py_BuildValue("i", status);
}

static PyObject *wd_direct_get_msr(PyObject *self, PyObject *args)
{
  unsigned int msr;
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  msr = wd_read_msr();
  return Py_BuildValue("i", msr);
}

static PyObject *wd_direct_drain(PyObject *self, PyObject *args)
{
  unsigned int status;
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  status = wd_drain();
  return Py_BuildValue("i", status);
}

static PyObject *wd_direct_delay_microseconds(PyObject *self, PyObject *args)
{
  unsigned int amount;
  if (!PyArg_ParseTuple(args, "i", &amount)) {
    return NULL;
  }
  myDelayMicroseconds(amount);
  return Py_BuildValue("i", 0);
}

static PyObject *wd_direct_enable_my_delay_micros(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }
  /*
  initMicros();
  EnableMyMicros = TRUE;
  */
  return Py_BuildValue("i", 0);
}

static PyMethodDef wd_direct_methods[] = {
  {"init", wd_direct_init, METH_VARARGS, "Initialize"},
  {"deinit", wd_direct_deinit, METH_VARARGS, "Deinitialize"},
  {"reset", wd_direct_reset, METH_VARARGS, "Reset"},
  {"pulse_dack", wd_direct_pulse_dack, METH_VARARGS, "Pulse Dack"},
  {"get_tc", wd_direct_get_tc, METH_VARARGS, "Get TC"},
  {"get_dc", wd_direct_get_dc, METH_VARARGS, "Get DC"},
  {"set_addr", wd_direct_set_addr, METH_VARARGS, "Set a0 bit"},
  {"read_byte", wd_direct_read_byte, METH_VARARGS, "Read byte at cs,address"},
  {"write_byte", wd_direct_write_byte, METH_VARARGS, "Write byte at cs,address"},
  {"write_data", wd_direct_write_data, METH_VARARGS, "Write data reg"},
  {"write_command", wd_direct_write_command, METH_VARARGS, "Write data reg"},  
  {"write_dor", wd_direct_write_dor, METH_VARARGS, "Write dor reg"},
  {"write_dcr", wd_direct_write_dcr, METH_VARARGS, "Write dcr reg"},
  {"read_block", wd_direct_read_block, METH_VARARGS, "Read block"},
  {"write_block", wd_direct_write_block, METH_VARARGS, "Write block"},
  {"read_result", wd_direct_read_result, METH_VARARGS, "Read command result"},
  {"get_msr", wd_direct_get_msr, METH_VARARGS, "get the msr"},
  {"wait_msr", wd_direct_wait_msr, METH_VARARGS, "wait for msr"},
  {"delay_microseconds", wd_direct_delay_microseconds, METH_VARARGS, "delay microseconds"},
  {"drain", wd_direct_drain, METH_VARARGS, "drain data"},
  {NULL, NULL, 0, NULL}
};
/*
static struct PyModuleDef wd37c65_direct_ext =
{
    PyModuleDef_HEAD_INIT,
    "wd37c65_direct_ext",   // name of module 
    "",                     // module documentation, may be NULL 
    -1,                     // size of per-interpreter state of the module, or -1 if the module keeps state in global variables. 
    wd_direct_methods
};

PyMODINIT_FUNC PyInit_wd37c65_direct_ext(void)
{
    return PyModule_Create(&wd37c65_direct_ext);
}
*/
PyMODINIT_FUNC initwd37c65_direct_ext(void)
{
  (void) Py_InitModule("wd37c65_direct_ext", wd_direct_methods);
}