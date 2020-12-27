#!/bin/bash
rm -f valgrind-out.txt
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./cpu_test
