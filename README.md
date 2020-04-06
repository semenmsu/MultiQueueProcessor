### MultiQueueProcessor MVS

mutlithreading

# MutliQueueProcessor

### Settings

-   MaxCapacity - максимальное количество в одной очереди
-   Persist - сохнанять или нет сообщения в очереди, если в данный момент нет подписчиков

### Test

-   хранятся в result/log{номер теста}.log
-   запуск ./run_test.sh

### Test1

передается int32_t при разном количестве producers and consumers,
оценивается количество переданных сообщений

### Test2

передаётся Value8192(8192 байта) при разном количестве producers and consumers,
оценивается количество переданных сообщений

### Test3

передаётся int32_t при разном количестве producers and consumers,
consumers случайным образом подписываются и отписываются

### Test4

передаётся Value8192(8192 байта) при разном количестве producers and consumers,
consumers случайным образом подписываются и отписываются

### Docker

-   ./build.sh (собрать docker, возможно потребуется chmod +x build.sh)
-   ./run.docker.test.sh (запустить docker, возможно потребуется chmod +x build.sh)
-   если всё успешно, то тесты сохраняется в ./docker_result директорию
