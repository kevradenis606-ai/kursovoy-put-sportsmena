using System;

namespace Task06
{
    /// <summary>
    /// Задание 6 (вариант 11). Вывести все целые числа из диапазона от A до B (A ≤ B),
    /// оканчивающиеся на любую нечётную цифру.
    ///
    /// Используется цикл for: количество повторений заранее известно,
    /// а счётчик удобно применять как само выводимое число.
    /// </summary>
    internal static class Program
    {
        private static void Main()
        {
            Console.Write("Введите число A: ");
            int a = ReadInt();
            Console.Write("Введите число B (B ≥ A): ");
            int b = ReadInt();

            if (a > b)
            {
                Console.WriteLine("Ошибка: число A должно быть меньше или равно B.");
                return;
            }

            Console.Write("Числа, оканчивающиеся на нечётную цифру: ");
            for (int i = a; i <= b; i++)
            {
                // Последняя цифра числа (для отрицательных чисел берём модуль).
                int lastDigit = Math.Abs(i % 10);
                if (lastDigit % 2 != 0)
                {
                    Console.Write(i + " ");
                }
            }

            Console.WriteLine();
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
