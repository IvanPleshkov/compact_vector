# compact_vector

WORK IN PROGRESS

Компакнтый вектор. Контейнер, реализующий интерфейс std::vector с применением технологии small string optimization.

Подробнее. Обычно вектор стандартной библиотеки имеет три поля: указатель/индекс для задания begin, end и capacity (где указатель, а где индекс зависит от реализации стандартной библиотеки). Следует отметить, что два указателя на x64 - это 16 байт, в которые вполне можно уместить небольшой вектор. Данный контейнер позволяет использовать стек для хранения небольшого количества данных, и кучу в противном случае. Размещение данных на стеке или куче производится автоматически.

Использование аналогично использованию std::vector со следующими отличиями:
- compact_vector имеет дополнительный параметр в шаблоне - compact_max_size. Это значение задает количество элементов, которые умещаются в стеке. compact_max_size не может быть меньше единицы, в противном случае вычисляется автоматически из sizeof(void*) и sizeof(size_t);
- максимальный размер контейнера ограничен значением std::vector::max_size() / 2;
- возможное проседание перфоманса вследствие дополнительных проверок на источник данных (стек или куча), перемещения данных из стека в кучу и обратно и, в целом, из-за пропущенных автором техник оптимизации;

