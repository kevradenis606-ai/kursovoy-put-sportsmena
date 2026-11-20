using System;

namespace Task03
{
    /// <summary>
    /// Задание 3 (вариант 11). Вычислить значение функции
    ///     y = x / ((x - 15) * (x + 2))
    /// для произвольного значения аргумента x.
    /// </summary>
    internal static class Program
    {
        private static void Main()
        {
            Console.Write("x = ");
            double x = ReadDouble();

            // Знаменатель обращается в ноль при x = 15 и x = -2.
            if (x == 15 || x == -2)
            {
                Console.WriteLine("Ошибка: деление на ноль (x = 15 или x = -2).");
                return;
            }

            double y = x / ((x - 15) * (x + 2));
            Console.WriteLine($"y = {y}");
        }

        private static double ReadDouble()
        {
            while (true)
            {
                if (double.TryParse(Console.ReadLine(), out double value))
                {
                    return value;
                }

                Console.Write("Ошибка: введите число. x = ");
            }
        }
    }
}
