using System;

namespace Task02
{
    /// <summary>
    /// Задание 2 (вариант 11). Определить, заканчивается ли сумма цифр
    /// трёхзначного числа на 2.
    /// </summary>
    internal static class Program
    {
        private static void Main()
        {
            int number = ReadThreeDigitNumber();
            int n = Math.Abs(number);

            int hundreds = n / 100;
            int tens = (n / 10) % 10;
            int units = n % 10;
            int sum = hundreds + tens + units;

            Console.WriteLine($"Число: {number}");
            Console.WriteLine($"Сумма цифр: {hundreds} + {tens} + {units} = {sum}");

            // Условие задачи: последняя цифра суммы равна 2.
            string result = sum % 10 == 2
                ? "заканчивается на 2"
                : "не заканчивается на 2";

            Console.WriteLine($"Результат: {result}");
        }

        /// <summary>Считывает трёхзначное число (по модулю от 100 до 999).</summary>
        private static int ReadThreeDigitNumber()
        {
            while (true)
            {
                Console.Write("Введите трёхзначное число: ");
                if (int.TryParse(Console.ReadLine(), out int value) && Math.Abs(value) is >= 100 and <= 999)
                {
                    return value;
                }

                Console.WriteLine("Ошибка: число должно быть трёхзначным (от 100 до 999 по модулю).");
            }
        }
    }
}
