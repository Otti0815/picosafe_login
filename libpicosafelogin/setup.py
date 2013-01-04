from distutils.core import setup, Extension

# to install run
# sudo python setup.py install

MODULE      = "picosafelogin"
NAME        = "picosafelogin library for python"
LIBRARIES   = ["usb"]
SOURCES     = ["base64.c", "libpicosafelogin.c", "libpicosafelogin-py.c", "rijndael.c"]
VERSION     = "1.0"
DESCRIPTION = "picosafelogin package"


module = Extension(MODULE, libraries=LIBRARIES, sources=SOURCES)

setup(name = NAME,
      version = VERSION,
      description = DESCRIPTION,
      ext_modules = [module])
