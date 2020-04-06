### MultiQueueProcessor MVS

mutlithreading

### Test

- хранятся в result/log{номер теста}.log
- запуск ./run_test.sh

### Test1

- передается int32_t при разном количестве producers and consumers,
  оценивается количество переданных сообщений
- передаётся Value64(64 байта) при разном количестве producers and consumers,
  оценивается количество переданных сообщений

### Test2

- в одном треде создаётся несколько producers и создаётся несколько тредом. Максимальная нагрузка на систему

### Test3

consumers случайным образом подписываются и отписываются

### Test4

consumers случайным образом создаются и удаляются

### Docker

- ./build.sh (собрать docker, возможно потребуется chmod +x build.sh)
- ./run.docker.test.sh (запустить docker, возможно потребуется chmod +x build.sh)
- если всё успешно, то тесты сохраняется в ./docker_result директорию
