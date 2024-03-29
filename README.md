# memcached

Автор: Карамнова Юлия

<b>Memcached</b> — программное обеспечение, реализующее сервис кэширования данных в оперативной памяти на основе хеш-таблицы.

С помощью клиентской библиотеки позволяет кэшировать данные в оперативной памяти множества доступных серверов. Распределение реализуется путем сегментирования данных по значению хэша ключа. Клиентская библиотека, используя ключ данных, вычисляет хэш и использует его для выбора соответствующего сервера.

<b>LRU cache</b>

<i>Алгоритм работы</i>

LRU cache реализован через 5 std::unordered_map и std::deque.
В 4 unordered_map хранят инофрмацию, std::deque хранит элементы в таком порядке, что в начале лежат указатели на ключи элементов, которые относительно давно не запрашивались, а в конце элементы, котороые вызывались совсем недавно, последний unordered_map хранит пары {key, позиция этого элемента в std::deque}.


Обращение, удаление, нахождение и добавление элемента по key в std::unordered_map реализовано за O(1).

Вставка элемента в конец и удаление элемента из начала в std::deque так же реалзовано за O(1).

<i>get</i>

1. Проверка, есть ли элемент с этим ключем. Нет - возвращаем false.
2. Присваивает поданным элементам нужные значения.
3. Отправляем указатель на key в конец std::deque и обнавляем значение пары {key, позиция этого элемента в std::deque}.

find - O(1), 
доступ по key - O(1), 
добавление элемента в конец в std::deque - O(1), 
добавление элемента по key - O(1).

O(1) + 4 * O(1) + O(1) + O(1) = O(1)


<i>Удаление элемента</i>

1. Проверка, есть ли элемент с этим ключем. Нет - возвращаем false.
2. Удаление из std::deque, а потом из всех std::unordered_map.

find - O(1), 
erase в std::unordered_map и std::deque - O(1).

O(1) + 6 * O(1) = O(1)


<i>set</i>

1. Проверка, есть ли элемент с этим ключем. Если есть, то удаляем.
2. Проверка, может ли кэш вместить новый элемент. Если нет, то удаляем первый элемент в std::deque из всех контейнеров.
3. Добавляем новые элементы во все контейнеры.

Удаление элемента - O(1), 
добавление элемента в конец в std::deque - O(1), 
добавление элемента по key - O(1).

O(1) + O(1) + 5 * O(1) + O(1) = O(1)

| 1000 | 10000 | 100000 |
|:-----------:|:------------:|:------------:|
|2.204 ms | 1.8313 ms| 1.786 ms|

Оценка.

4 балла. Сервер способен выполнять команды "set", "add", "get", "delete". Реализован LRU кеш.

+1 балл. Асимптотика O(1) для выполнения операций в классе Cache.

+1 балл. За остановку сервера после 5 секунд ожидания.
