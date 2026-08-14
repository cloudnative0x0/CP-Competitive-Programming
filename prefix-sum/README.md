# Prefix Sum

[Русский](#русский) | [English](#english)

---

## Русский

### Описание

Массив, где `prefix[i]` хранит сумму первых `i` элементов исходного массива. Строится один раз за `O(n)`, дальше любой запрос суммы отрезка `[left, right]` — это разность двух чисел, `prefix[right+1] - prefix[left]`.

Работает это потому, что `prefix[right+1]` уже включает в себя всё, что входит в `prefix[left]`, плюс кусок от `left` до `right`. Вычитая `prefix[left]`, убираешь лишний хвост слева и остаётся ровно нужный отрезок.

Смысл паттерна — перенести цену запроса с "прочитать отрезок целиком" на "прочитать одну ячейку и вычесть другую". Ты платишь один раз при построении и потом каждый запрос почти бесплатный. Это имеет смысл только когда запросов много и массив не меняется между ними — если массив меняется после каждого запроса, придётся пересчитывать всё заново, и весь выигрыш пропадает.

### Хитрость: как распознать в задаче этот паттерн

- В условии несколько (часто десятки тысяч) запросов вида "сумма/среднее/количество на отрезке `[left, right]`" — единичный запрос смысла в префиксных суммах не имеет, обычный цикл окажется и проще, и быстрее по факту (меньше константы).
- Массив статичный: обновлений между запросами нет или они редкие. Если элементы массива меняются, а запросы суммы идут вперемешку с изменениями — нужен Fenwick tree (BIT) или дерево отрезков, префиксные суммы тут не подойдут: обновление одного элемента заставит пересчитать весь хвост массива.
- Часто маскируется под "количество чисел, удовлетворяющих условию, в диапазоне" — тогда строится не сумма самих чисел, а сумма индикаторов (0/1), и это тот же паттерн один в один.
- Двумерный вариант того же трюка узнаётся по "сумма подматрицы" — там `prefix[i][j]` и формула с включением-исключением по четырём углам.
- Ключевые слова в условии: "range sum query", "cumulative", "накопительная сумма", "сумма на отрезке", "static array".

### Пояснение кода и как работает

```cpp
vector<long long> buildPrefixSum(const vector<long long>& arr) {
    vector<long long> prefix(arr.size() + 1, 0);

    for (size_t i = 0; i < arr.size(); i++) {
        prefix[i+1] = prefix[i] + arr[i];
    }

    return prefix;
}
```

Размер `prefix` — `arr.size() + 1`, а не `arr.size()`. Это не случайность, а то, что убирает половину edge-кейсов: `prefix[0] = 0` означает "сумма нуля элементов", и запрос `rangeSum(prefix, 0, r)` работает без отдельной проверки на `l == 0`.

```cpp
long long rangeSum(const vector<long long>& prefix, int l, int r) {
    return prefix[r+1] - prefix[l];
}
```

`l` и `r` — 0-indexed, отрезок включительный с обеих сторон. Если задача даёт индексы с 1, их нужно сдвинуть на входе, а не переписывать формулу.

Функция не проверяет границы — если `r+1` выйдет за пределы `prefix.size()` или `l < 0`, будет неопределённое поведение (`operator[]` у `vector` границы не проверяет). В спортивном программировании это осознанное решение: границы гарантирует условие задачи, а проверки в hot path — лишняя работа. Если нужна проверка на этапе отладки — используй `prefix.at(...)` вместо `prefix[...]`, он бросит `std::out_of_range`, но за это платишь на каждом обращении, поэтому в финальную версию его не оставляют. При написании стресс-теста это ровно то место, где легко получить некорректный тест, если генератор выдаст `l > r` или `r >= n`.

### Асимптотика

| Операция | Время | Память |
|---|---|---|
| Построение `prefix` | `O(n)` | `O(n)` (доп. массив, оригинальный `arr` не трогается) |
| Один запрос `rangeSum` | `O(1)` | — |
| `q` запросов после построения | `O(n + q)` суммарно (против `O(n * q)` наивным циклом) | — |

Порог, где префиксные суммы начинают реально выигрывать — где-то от нескольких сотен запросов на массив в несколько тысяч элементов. На малых `n` и `q` разница не будет заметна, а код станет на порядок сложнее ради теплового шума в бенчмарке.

---

## English

### Description

An array where `prefix[i]` stores the sum of the first `i` elements of the source array. Built once in `O(n)`, after that any range sum query `[left, right]` is just the difference of two numbers: `prefix[right+1] - prefix[left]`.

This works because `prefix[right+1]` already includes everything contained in `prefix[left]`, plus the chunk from `left` to `right`. Subtracting `prefix[left]` removes the extra tail on the left, leaving exactly the range you need.

The point of the pattern is to move the cost of a query from "read the whole range" to "read one cell and subtract another." You pay once at build time, and after that every query is nearly free. This only pays off when there are many queries and the array doesn't change between them — if the array is mutated after every query, you'd have to rebuild from scratch each time, and the whole win disappears.

### The trick: spotting this pattern in a problem

- The problem has several (often tens of thousands of) queries of the form "sum/average/count over range `[left, right]`" — a single one-off query gains nothing from prefix sums; a plain loop is both simpler and, in practice, faster (smaller constant factor).
- The array is static: no updates between queries, or updates are rare. If elements change while sum queries are interleaved with updates, you need a Fenwick tree (BIT) or a segment tree instead — prefix sums break down, since updating one element forces recomputing the whole tail of the array.
- Often disguised as "count of numbers satisfying a condition within a range" — then you build a sum not of the numbers themselves but of 0/1 indicators, and it's exactly the same pattern.
- The 2D version of the same trick shows up as "sum of a submatrix" — there you get `prefix[i][j]` and an inclusion-exclusion formula over four corners.
- Keywords in the statement: "range sum query", "cumulative", "sum over a range", "static array".

### Code walkthrough

```cpp
vector<long long> buildPrefixSum(const vector<long long>& arr) {
    vector<long long> prefix(arr.size() + 1, 0);

    for (size_t i = 0; i < arr.size(); i++) {
        prefix[i+1] = prefix[i] + arr[i];
    }

    return prefix;
}
```

The size of `prefix` is `arr.size() + 1`, not `arr.size()`. That's deliberate — it eliminates half the edge cases: `prefix[0] = 0` means "sum of zero elements," so the query `rangeSum(prefix, 0, r)` works without a separate check for `l == 0`.

```cpp
long long rangeSum(const vector<long long>& prefix, int l, int r) {
    return prefix[r+1] - prefix[l];
}
```

`l` and `r` are 0-indexed, and the range is inclusive on both ends. If the problem uses 1-indexed input, shift the indices at the boundary — don't rewrite the formula.

The function doesn't check bounds — if `r+1` goes past `prefix.size()` or `l < 0`, you get undefined behavior (`vector::operator[]` doesn't bounds-check). In competitive programming this is a deliberate choice: bounds are guaranteed by the problem statement, and checks in the hot path are wasted work. If you want a check while debugging, swap in `prefix.at(...)` — it throws `std::out_of_range` — but you pay for that on every access, so it doesn't belong in the final version. When writing a stress test, this is exactly where it's easy to generate an invalid case if the generator produces `l > r` or `r >= n`.

### Complexity

| Operation | Time | Memory |
|---|---|---|
| Building `prefix` | `O(n)` | `O(n)` (extra array; original `arr` untouched) |
| One `rangeSum` query | `O(1)` | — |
| `q` queries after building | `O(n + q)` total (vs. `O(n * q)` naive loop) | — |

The threshold where prefix sums actually start paying off is roughly a few hundred queries against an array of a few thousand elements. For small `n` and `q`, the difference is unnoticeable, and the code becomes an order of magnitude more complex just to shave off benchmark noise.

---

*Premature optimization is the root of all evil. — Donald Knuth*