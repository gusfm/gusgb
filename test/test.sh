#!/bin/bash
rm -f *valgrind-out.txt
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=cart_test-valgrind-out.txt ./cart_test
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=cpu_test-valgrind-out.txt ./cpu_test
