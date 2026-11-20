using System;

namespace Task01
{
    /// <summary>
    /// Задание 1. Диалог с пользователем: программа запрашивает с клавиатуры
    /// числа и выводит на экран их сумму.
    /// </summary>
    internal static class Program
    {
        private static void Main()
        {
            Console.WriteLine("Задание 1. Сумма чисел, введённых с клавиатуры.");
            Console.WriteLine();

            double a = ReadDouble("a = ");
            double b = ReadDouble("b = ");
            Console.WriteLine($"{a} + {b} = {a + b}");
            Console.WriteLine();

            double c = ReadDouble("c = ");
            double d = ReadDouble("d = ");
            Console.WriteLine($"{c} + {d} = {c + d}");
            Console.WriteLine();

            double e = ReadDouble("e = ");
            double f = ReadDouble("f = ");
            double g = ReadDouble("g = ");
            Console.WriteLine($"{e} + {f} + {g} = {e + f + g}");
        }

        /// <summary>Считывает число с клавиатуры, повторяя запрос при ошибке ввода.</summary>
        private static double ReadDouble(string prompt)
        {
            while (true)
            {
                Console.Write(prompt);
                if (double.TryParse(Console.ReadLine(), out double value))
                {
                    return value;
                }

                Console.WriteLine("Ошибка: введите число.");
            }
        }
    }
}
