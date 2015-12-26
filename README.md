# memcached

Автор: Карамнова Юлия

<b>Memcached</b> — программное обеспечение, реализующее сервис кэширования данных в оперативной памяти на основе хеш-таблицы.

С помощью клиентской библиотеки позволяет кэшировать данные в оперативной памяти множества доступных серверов. Распределение реализуется путем сегментирования данных по значению хэша ключа. Клиентская библиотека, используя ключ данных, вычисляет хэш и использует его для выбора соответствующего сервера.

<b>LRU cache</b>

| 1000 | 10000 | 100000 |
|:-----------:|:------------:|:------------:|
|2.204 ms | 1.8313 ms| 1.786 ms|

LRU cache реализован через 5 std::unordered_map и std::deque.
Обращение, удаление, нахождение и добавление элемента по key в std::unordered_map реализовано за O(1).

Вставка элемента в конец и удаление элемента из начала в std::deque так же реалзовано за O(1).

<i>get</i>

find - O(1), 
доступ по key - O(1), 
добавление элемента в конец в std::deque - O(1), 
добавление элемента по key - O(1).

O(1) + 4 * O(1) + O(1) + O(1) = O(1)

<i>Удаление элемента</i>

find - O(1), 
erase в std::unordered_map и std::deque - O(1).

O(1) + 6 * O(1) = O(1)

<i>set</i>

Удаление элемента - O(1), 
добавление элемента в конец в std::deque - O(1), 
добавление элемента по key - O(1).

O(1) + O(1) + 5 * O(1) + O(1) = O(1)