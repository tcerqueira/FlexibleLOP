#!/bin/sh

echo 'sending OrderTest.xml'
nc -u -w 1 127.0.0.1 54321 < OrderTest1.xml
nc -u -w 1 127.0.0.1 54321 < II_comands_A_v1/command5.xml
