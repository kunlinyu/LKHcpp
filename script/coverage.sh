#!/bin/bash

lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/include/*' 'include/CLI11' 'include/nlohmann' 'include/plog' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_html
