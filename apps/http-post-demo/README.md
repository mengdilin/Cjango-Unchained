##
This demo web server has a dependency on sqlite3.

```
sudo apt-get install libsqlite3-dev
```
Alternatively, for your convenience, a snapshot of sqlite3 is included in this directory. To build,
```
cd sqlite-snapshot-201704181120/
./configure --prefix=/usr/local && make && sudo make install
```
