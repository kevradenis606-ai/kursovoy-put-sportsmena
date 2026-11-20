using System;

namespace Task05
{
    /// <summary>
    /// Задание 5 (вариант 1). Дан порядковый номер месяца — вывести его название.
    ///
    /// Используется оператор switch: он позволяет выбрать одну из нескольких ветвей
    /// по значению целого числа и читается нагляднее, чем цепочка вложенных if/else.
    /// </summary>
    internal static class Program
    {
        private static void Main()
        {
            Console.Write("Введите порядковый номер месяца (1–12): ");
            int month = ReadInt();

            string name = month switch
            {
                1 => "Январь",
                2 => "Февраль",
                3 => "Март",
                4 => "Апрель",
                5 => "Май",
                6 => "Июнь",
                7 => "Июль",
                8 => "Август",
                9 => "Сентябрь",
                10 => "Октябрь",
                11 => "Ноябрь",
                12 => "Декабрь",
                _ => string.Empty
            };

            if (name.Length == 0)
            {
                Console.WriteLine("Ошибка: номер месяца должен быть от 1 до 12.");
            }
            else
            {
                Console.WriteLine(name);
            }
        }

        private static int ReadInt()
        {
            while (true)
            {
                if (int.TryParse(Console.ReadLine(), out int value))
                {
                    return value;
                }

                Console.Write("Ошибка: введите целое число: ");
            }
        }
    }
}
