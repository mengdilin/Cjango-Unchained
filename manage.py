
#!/usr/bin/env python
import os
import sys

if __name__ == "__main__":

  # If you don't install our libhttp_response.so and libhttp_request.so
  # in the place where linker can reference at runtime of ./manage,
  # it will cause error (checked on Ubuntu 16.10)
  libhttp_path = os.path.dirname(os.path.abspath(__file__)) + "/src/http_parser/"
  os.environ['LD_LIBRARY_PATH'] =  libhttp_path

  if len(sys.argv) > 1 and sys.argv[1] == 'runserver':
    port_number = sys.argv[2] if len(sys.argv) > 2 else '8000'
    for i in range(len(sys.argv)):
      if sys.argv[i] == "--setting":
        sys.argv[i+1] = "../" + sys.argv[i+1]

    if len(sys.argv) > 3:
      others = " " + " ".join(sys.argv[3:])
    else:
      others = ""
    # FIXME: ld error of Referenced from: /Users/PCUser/Dropbox/CU2017/S2017CLASSES/CPP_Stroustrup/Cjango-Unchained/./src/manage
    # cd is a quick fix
    os.system('cd src && ./manage runserver ' + port_number + others)
  else:
    print("usage: python manage.py runserver <port number (default:8000)>")