
#!/usr/bin/env python
import os
import sys

if __name__ == "__main__":
  if len(sys.argv) > 1 and sys.argv[1] == 'runserver':
    port_number = sys.argv[2] if len(sys.argv) > 2 else '8000'
    if len(sys.argv) > 3:
      others = " " + " ".join(sys.argv[3:])
    else:
      others = ""
    # FIXME: ld error of Referenced from: /Users/PCUser/Dropbox/CU2017/S2017CLASSES/CPP_Stroustrup/Cjango-Unchained/./src/runapp
    # cd is a quick fix
    os.system('cd src && ./runapp runserver ' + port_number + others)
  else:
    print("usage: python manage.py runserver <port number (default:8000)>")