## Dependencies
This application uses sqlite3 as its backend database. To install sqlite3,

```
sudo apt-get install libsqlite3-dev
```
Alternatively, for your convenience, a snapshot of sqlite3 is included in this directory. To build and install,

```
cd sqlite-snapshot-201704181120/
./configure --prefix=/usr/local
make
sudo make install
```

## To Run
```
cd ../../
make -C demo/http-post-demo/callbacks
python manage.py runserver 8000 --setting apps/http-post-demo/json/settings.json

```
