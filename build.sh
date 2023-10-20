#!/bin/bash


cd ./memory_pool
make clean
make
make install
cd ..


cd ./dissect_engine
make clean
make
make install
cd ..


cd ./proto_identify
make clean
make
make install
cd ..


cd ./proto_dissect/oicq
make clean
make
make install
cd ../../


cd ./proto_dissect/http
make clean
make
make install
cd ../../


make clean
make
