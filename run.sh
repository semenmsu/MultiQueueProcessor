#!/bin/bash
docker run -it --entrypoint "/bin/bash" -v $PWD/docker_result/:/MutliQueueProcessor/result/ semenmsu/multiqueueprocessor:1.0