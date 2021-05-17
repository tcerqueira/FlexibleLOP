#!/bin/sh

echo 'sending OrderTest.xml'
nc -u -w 1 127.0.0.1 54321 < OrderTest.xml
