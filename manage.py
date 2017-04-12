#!/usr/bin/env python
import os
import sys

if __name__ == "__main__":
  if len(sys.argv) > 1 and sys.argv[1] == 'runserver':
    port_number = sys.argv[2] if len(sys.argv) > 2 else '8000'
    os.system('./runapp runserver ' + port_number)
  else:
    print("usage: python manage.py runserver <port number (default:8000)>")