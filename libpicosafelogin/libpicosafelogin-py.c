/* headers */
#include <Python.h>
#include "libpicosafelogin.h"
#include "../firmware/addresses.h"


PyObject *set_owner(PyObject *self, PyObject *args)
{
  int ret;
  char *owner;

  if (!PyArg_ParseTuple(args, "s", &owner))
    return NULL;

  ret = picosafelogin_set_owner(owner);
  return Py_BuildValue("i", ret);
}


PyObject *get_owner(PyObject *self, PyObject *args)
{
  int ret;
  char owner[LEN_OWNER];

  memset(owner, 0, sizeof(owner));

  ret = picosafelogin_get_owner(owner);
  return Py_BuildValue("s", owner);
}


PyObject *get_serial(PyObject *self, PyObject *args)
{
  int ret, i;
  char serial[11];
  char output[33];

  memset(serial, 0, sizeof(serial));
  memset(output, 0, sizeof(output));

  ret = picosafelogin_get_internal_serial(serial);

  for(i = 0; i < 11; i++)
    snprintf(&output[i*2], 3, "%02X", (uint8_t)serial[i]);

  return Py_BuildValue("s", output);
}


PyObject *set_company(PyObject *self, PyObject *args)
{
  int ret;
  char *company;

  if (!PyArg_ParseTuple(args, "s", &company))
    return NULL;

  ret = picosafelogin_set_company(company);
  return Py_BuildValue("i", ret);
}


PyObject *get_company(PyObject *self, PyObject *args)
{
  int ret;
  char company[LEN_COMPANY];

  memset(company, 0, sizeof(company));

  ret = picosafelogin_get_company(company);
  return Py_BuildValue("s", company);
}


PyObject *set_seed(PyObject *self, PyObject *args)
{
  int ret;
  char *seed;

  if (!PyArg_ParseTuple(args, "s", &seed))
    return NULL;

  ret = picosafelogin_set_seed(seed);
  return Py_BuildValue("i", ret);
}


PyObject *set_description(PyObject *self, PyObject *args)
{
  int ret;
  char *desc;

  if (!PyArg_ParseTuple(args, "s", &desc))
    return NULL;

  ret = picosafelogin_set_description(desc);
  return Py_BuildValue("i", ret);
}


PyObject *get_description(PyObject *self, PyObject *args)
{
  int ret;
  char desc[LEN_DESCRIPTION];

  memset(desc, 0, sizeof(desc));

  ret = picosafelogin_get_description(desc);
  return Py_BuildValue("s", desc);

}


PyObject *set_id(PyObject *self, PyObject *args)
{
  int ret;
  long int id;

  if (!PyArg_ParseTuple(args, "l", &id))
    return NULL;

  ret = picosafelogin_set_id((uint64_t)id);
  return Py_BuildValue("i", ret);
}


PyObject *get_id(PyObject *self, PyObject *args)
{
  uint32_t id = 0;

  picosafelogin_get_id(&id);

  return Py_BuildValue("i", id);
}


PyObject *get_time(PyObject *self, PyObject *args)
{
  uint32_t time = 0;

  picosafelogin_get_time(&time);

  return Py_BuildValue("i", time);
}


PyObject *get_temperature(PyObject *self, PyObject *args)
{
  int temp;

  temp = picosafelogin_get_temperature();
  return Py_BuildValue("i", temp);

}


PyObject *set_lock(PyObject *self, PyObject *args)
{
  int ret;

  ret = picosafelogin_set_lock();
  return Py_BuildValue("i", ret);
}


PyObject *verify_token(PyObject *self, PyObject *args)
{
  const char *pad, *pw;
  int num, ret;
  uint32_t timestamp;

  if (!PyArg_ParseTuple(args, "ss", &pw, &pad))
    return NULL;

  ret = picosafelogin_verify_token(pad, pw, &num, &timestamp);

  if(ret > 0)
    return Py_BuildValue("iii", ret, num, timestamp);
  else
    return Py_BuildValue("i", ret);
}


PyObject *get_nonce(PyObject *self, PyObject *args)
{
  int ret;
  char nonce[120];

  memset(nonce, 0, sizeof(nonce));

  ret = picosafelogin_get_nonce(nonce, sizeof(nonce));

  return Py_BuildValue("s", nonce);
}


PyObject *set_timestamp(PyObject *self, PyObject *args)
{
  int ret;
  uint32_t timestamp = 0;
  const char *pw;

  if (!PyArg_ParseTuple(args, "is", &timestamp, &pw))
    return NULL;

  ret = picosafelogin_set_timestamp(timestamp, pw);

  return Py_BuildValue("i", ret);
}


PyObject *reset(PyObject *self, PyObject *args)
{
  int ret;
  char *master_password;

  if (!PyArg_ParseTuple(args, "s", &master_password))
    return NULL;

  ret = picosafelogin_reset(master_password);

  return Py_BuildValue("i", ret);
}




/* Module functions */

static PyMethodDef methods[] = {
  {"set_owner",       set_owner,       METH_VARARGS},
  {"get_owner",       get_owner,       METH_VARARGS},
  {"get_serial",      get_serial,      METH_VARARGS},
  {"set_company",     set_company,     METH_VARARGS},
  {"get_company",     get_company,     METH_VARARGS},
  {"set_seed",        set_seed,        METH_VARARGS},
  {"set_description", set_description, METH_VARARGS},
  {"get_description", get_description, METH_VARARGS},
  {"set_id",          set_id,          METH_VARARGS},
  {"get_id",          get_id,          METH_VARARGS},
  {"get_time",        get_time,        METH_VARARGS},
  {"get_temperature", get_temperature, METH_VARARGS},
  {"set_lock",        set_lock,        METH_VARARGS},
  {"verify_token",    verify_token,    METH_VARARGS},
  {"get_nonce",       get_nonce,       METH_VARARGS},
  {"set_timestamp",   set_timestamp,   METH_VARARGS},
  {"reset",           reset,           METH_VARARGS},
  {NULL, NULL},
};

/* Module init function */

void initpicosafelogin(void)
{   PyObject *m, *d;

    m = Py_InitModule("picosafelogin", methods);
    d = PyModule_GetDict(m);

    /* initialize module variables/constants */
}
