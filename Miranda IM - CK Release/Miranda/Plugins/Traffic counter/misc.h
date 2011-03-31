// Функция заменяет фигурные скобки в строке string на пробелы
void ReplaceBrackets(TCHAR *string);

// Функция принимает строку с фигурными скобками и возвращает строки, предназначенные для
// вывода с выравниванием по левому/центру/по правому
void GetFormattedStrings(TCHAR *src, TCHAR *la, TCHAR *ca, TCHAR *ra);

/* Функция возвращает количество дней в указанном месяце указанного года. */
BYTE DaysInMonth(BYTE Month, WORD Year);

// Функция определяет день недели по дате
// 7 - ВС, 1 - ПН и т. д.
BYTE DayOfWeek(BYTE Day, BYTE Month, WORD Year);

/*
Аргументы:
Value - количество байт;
Unit - единицы измерения (0 - байты, 1 -килобайты, 2 - мегабайты, 3 - автоматически);
Buffer - адрес строки для записи результата
*/
void GetFormattedTraffic(DWORD Value, BYTE Unit, TCHAR *Buffer);

void GetDurationFormatM(DWORD Duration, TCHAR *Format, TCHAR *Buffer, WORD Size);

signed short int TimeCompare(SYSTEMTIME st1, SYSTEMTIME st2);