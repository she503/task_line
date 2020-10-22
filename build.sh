#!/bin/bash

set -ue

TERGEO_LIB_PATH=$(cd `dirname $0`; pwd)

source /tergeo/bash/tergeo_build.sh

tergeo_build