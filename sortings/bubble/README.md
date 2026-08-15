# Bubble Sort

[Русский](#русский) | [English](#english)

---

## Русский

### Описание

Сортировка сравнивает соседние элементы и меняет их местами, если порядок нарушен. За один проход по массиву самый большой (или самый маленький, в зависимости от компаратора) элемент гарантированно оказывается на своём месте с конца — он "всплывает" за один проход, отсюда и название. Проходов нужно до `n`, но если на очередном проходе не случилось ни одного обмена, массив уже отсортирован и можно остановиться раньше — это и есть строчка `swapped`.

Внутренний цикл на каждом проходе укорачивается: `j + 1 < collection.size() - i`. После `i` проходов последние `i` элементов уже стоят на своих местах, повторно их трогать незачем.

Сортировка устойчивая (equal элементы не меняют взаимный порядок, потому что обмен происходит строго при строгом нарушении, а не при равенстве) и на месте (`in-place`, дополнительная память не нужна, в отличие от merge sort).

### Хитрость: как распознать в задаче этот паттерн

- В условии прямым текстом просят количество обменов/операций, которые понадобятся, чтобы отсортировать массив через обмен соседних элементов — это ровно то, что считает пузырёк, и совпадает с числом инверсий в массиве.
- `n` в ограничениях маленькое, обычно до 1000–2000. При `O(n^2)` и `n = 2000` это 4 миллиона операций — укладывается в лимит по времени с запасом. При `n` в районе 10^5 и выше пузырёк не пройдёт по времени в принципе, там нужен `O(n log n)`: `std::sort`, merge sort, или, если задача про подсчёт инверсий на большом `n`, — Fenwick tree по сжатым координатам.
- Массив почти отсортирован (несколько элементов не на своих местах, остальное по порядку) — здесь пузырёк адаптивный: при раннем выходе по `swapped` реальное число проходов может быть маленьким, и на практике будет быстрее, чем ожидаемый `O(n^2)`.
- Задача требует минимального и понятного кода без сторонних структур данных, а вход заведомо мал — писать `std::sort` с кастомным компаратором тут не хуже, но если в условии прямо требуют реализовать сортировку "руками" (учебная задача, не спортивная), это тоже сигнал.
- Ключевые слова: "adjacent swap", "bubble sort", "количество обменов соседних элементов", "почти отсортированный массив", "small n".
- Обратный сигнал — если в условии `n` большое и просят просто отсортировать без доп. условий, пузырёк не нужен вообще, бери `std::sort`. Реализовывать его руками в спортивном программировании имеет смысл почти исключительно тогда, когда сам процесс обмена соседей — часть ответа, а не просто способ получить отсортированный массив.

### Пояснение кода и как работает

```cpp
template<typename Collection, typename Comparator, typename = typename Collection::size_type>
void bubble_sort(Collection& collection, Comparator comparator) noexcept {
    bool swapped = true;
    
    for (typename Collection::size_type i = 0; i < collection.size() && swapped; i++) {
        swapped = false;
        
        for (typename Collection::size_type j = 0; j + 1 < collection.size() - i; j++) {
```

Компаратор передаётся снаружи, а не зашит в тело функции — то же самое `bubble_sort` сортирует и по возрастанию, и по убыванию, и по любому кастомному порядку, в зависимости от того, что передашь. `comparator(collection[j], collection[j + 1])` возвращает `true`, когда пара стоит неправильно и нужен обмен: с `a > b` условие срабатывает, когда левый элемент больше правого, и после обмена он уходит вправо — итог: массив по возрастанию.

Третий параметр шаблона, `typename = typename Collection::size_type`, — SFINAE-проверка на этапе компиляции: если у переданного типа нет `size_type` (то есть это не похоже на STL-контейнер), шаблон не заинстанцируется, и вместо непонятной ошибки где-то в теле функции ты получишь ошибку подстановки уже на этапе выбора перегрузки.

`j + 1 < collection.size() - i` — граница проверяется через `j + 1 < ...`, а не `j < ... - 1`. Разница принципиальна: `collection.size()` беззнаковый (`size_type`), и если `i == collection.size()`, `collection.size() - i` даст `0`, а `0 - 1` для беззнакового типа — это переполнение в огромное положительное число. Запись `j + 1 < collection.size() - i` этой ловушки избегает.

```cpp
            if (comparator(collection[j], collection[j + 1])) {
                std::swap(collection[j], collection[j + 1]);
                swapped = true;
```

Флаг `swapped` — это и есть весь смысл ранней остановки. Без него алгоритм всегда делает `n` проходов, даже если массив отсортирован на нулевом. С флагом лучший случай (уже отсортированный вход) — это один проход и `O(n)`, а не `O(n^2)`.

Вызов помечен `noexcept`: обмен через `std::swap` для чисел и подобных типов не бросает исключений, и это честно отражено в сигнатуре. Если передать сюда тип с исключающим конструктором перемещения, это будет уже на совести вызывающего — шаблон такую проверку не делает.

Отладочные вызовы `pkg::Trace()` / `pkg::Debug(...)` не влияют на логику — это трассировка для профилировщика, включаемая отдельно, и в release-сборке при выключенном трейсе они вырождаются в проверку одного условия без побочных эффектов.

### Асимптотика

| Случай | Время | Память |
|---|---|---|
| Худший (обратный порядок) | `O(n^2)` | `O(1)` |
| Средний | `O(n^2)` | `O(1)` |
| Лучший (уже отсортирован, есть `swapped`) | `O(n)` | `O(1)` |
| Число проходов при почти отсортированном входе | пропорционально числу инверсий | `O(1)` |

Для сравнения: `std::sort` — гарантированный `O(n log n)` в среднем и худшем случае вне зависимости от входных данных. Пузырёк выигрывает у него только на очень маленьком `n` за счёт меньшей константы на проход и на почти отсортированных данных за счёт адаптивности через `swapped`. Во всех остальных случаях он проигрывает, и переписывать `std::sort` руками в спортивном программировании смысла нет.

---

## English

### Description

The algorithm compares adjacent elements and swaps them when they're in the wrong order. On a single pass, the largest (or smallest, depending on the comparator) remaining element is guaranteed to land in its final position at the end — it "bubbles up" over the course of that pass, hence the name. Up to `n` passes are needed, but if a pass completes with zero swaps, the array is already sorted and the algorithm can stop early — that's what the `swapped` flag tracks.

The inner loop shrinks on every pass: `j + 1 < collection.size() - i`. After `i` passes, the last `i` elements already sit in their final spots, so there's no reason to touch them again.

The sort is stable (equal elements keep their relative order, because a swap only happens on a strict violation, never on equality) and in-place (`O(1)` extra memory, unlike merge sort).

### The trick: spotting this pattern in a problem

- The problem literally asks for the number of adjacent swaps needed to sort the array — that's exactly what bubble sort counts, and it equals the number of inversions in the array.
- `n` in the constraints is small, usually up to 1000–2000. At `O(n^2)` with `n = 2000` that's 4 million operations, comfortably inside the time limit. Once `n` reaches around 10^5 or higher, bubble sort simply won't finish in time — that calls for `O(n log n)`: `std::sort`, merge sort, or, if the task is inversion counting on large `n`, a Fenwick tree over compressed coordinates.
- The array is nearly sorted already (a handful of elements out of place, the rest in order) — here bubble sort is adaptive: with the early exit on `swapped`, the actual number of passes can be small, and in practice it runs faster than the nominal `O(n^2)` suggests.
- The task asks for minimal, dependency-free code and the input is guaranteed small — `std::sort` with a custom comparator works just as well here, but if the statement explicitly asks you to implement sorting "by hand" (a teaching exercise rather than a competitive one), that's also a signal to reach for this.
- Keywords: "adjacent swap", "bubble sort", "number of adjacent-element swaps", "nearly sorted array", "small n".
- Reverse signal — if `n` is large and the problem just asks to sort the array with no extra conditions, skip bubble sort entirely and use `std::sort`. Implementing it by hand in competitive programming makes sense almost only when the swap process itself is part of the answer, not just a means to get a sorted array.

### Code walkthrough

```cpp
template<typename Collection, typename Comparator, typename = typename Collection::size_type>
void bubble_sort(Collection& collection, Comparator comparator) noexcept {
    bool swapped = true;
    
    for (typename Collection::size_type i = 0; i < collection.size() && swapped; i++) {
        swapped = false;
        for (typename Collection::size_type j = 0; j + 1 < collection.size() - i; j++) {
```

The comparator is passed in from outside rather than hardcoded — the same `bubble_sort` handles ascending, descending, or any custom order depending on what you pass. `comparator(collection[j], collection[j + 1])` returns `true` when the pair is in the wrong order and needs a swap: with `a > b`, the condition fires when the left element is bigger than the right one, and after the swap it moves right — the net result is ascending order.

The third template parameter, `typename = typename Collection::size_type`, is a compile-time SFINAE check: if the passed type has no `size_type` member (i.e. it doesn't look like an STL container), the template simply fails to instantiate, so you get a substitution error at overload resolution instead of a confusing error somewhere inside the function body.

The bound is checked as `j + 1 < collection.size() - i`, not `j < collection.size() - i - 1`. The difference matters: `collection.size()` is unsigned (`size_type`), and if `i == collection.size()`, then `collection.size() - i` evaluates to `0`, and `0 - 1` on an unsigned type wraps around to a huge positive number. Writing it as `j + 1 < collection.size() - i` sidesteps that trap.

```cpp
            if (comparator(collection[j], collection[j + 1])) {
                std::swap(collection[j], collection[j + 1]);
                swapped = true;
```

The `swapped` flag is the whole point of the early exit. Without it, the algorithm always runs `n` passes, even on an input that's already sorted. With it, the best case (already-sorted input) is a single pass and `O(n)`, not `O(n^2)`.

The function is marked `noexcept`: swapping via `std::swap` for numeric and similar types never throws, and the signature honestly reflects that. Pass in a type with a throwing move constructor, and that's on the caller — the template doesn't check for it.

The `pkg::Trace()` / `pkg::Debug(...)` calls don't affect the logic — they're profiler tracing, toggled separately, and in a release build with tracing off they collapse into a single condition check with no side effects.

### Complexity

| Case | Time | Memory |
|---|---|---|
| Worst (reverse order) | `O(n^2)` | `O(1)` |
| Average | `O(n^2)` | `O(1)` |
| Best (already sorted, `swapped` triggers) | `O(n)` | `O(1)` |
| Passes on a nearly-sorted input | proportional to the number of inversions | `O(1)` |

For comparison: `std::sort` guarantees `O(n log n)` on average and in the worst case, regardless of the input. Bubble sort only beats it on very small `n`, thanks to a smaller per-pass constant, and on nearly-sorted data, thanks to the adaptive `swapped` exit. In every other case it loses, and reimplementing `std::sort` by hand in competitive programming buys nothing.

---

*Premature optimization is the root of all evil. — Donald Knuth*