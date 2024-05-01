#!/bin/bash

oj-bundle main.cpp > submit.cpp
acc submit submit.cpp -- --no-open -y