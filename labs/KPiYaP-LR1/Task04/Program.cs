using System;

namespace Task04
{
    /// <summary>
    /// Задание 4 (вариант 11). Дана точка на плоскости с координатами (x, y).
    /// Программа выдаёт «Да», «Нет» или «На границе» в зависимости от того,
    /// лежит ли точка внутри заштрихованной области, вне её или на границе.
    ///
    /// Области (вариант 11):
    ///   1) прямоугольник (0; 5) × (3; 5);
    ///   2) прямоугольник (-7; 0) × (-5; -3).
    /// </summary>
    internal static class Program
    {
        private static void Main()
        {
            Console.Write("x = ");
            double x = ReadDouble();
            Console.Write("y = ");
            double y = ReadDouble();

            if (IsInside(x, y))
            {
                Console.WriteLine("Да");
            }
            else if (IsOnBorder(x, y))
            {
                Console.WriteLine("На границе");
            }
            else
            {
                Console.WriteLine("Нет");
            }
        }

        /// <summary>Точка строго внутри одной из заштрихованных областей.</summary>
        private static bool IsInside(double x, double y) =>
            (x > 0 && x < 5 && y > 3 && y < 5) ||
            (x > -7 && x < 0 && y > -5 && y < -3);

        /// <summary>Точка лежит на границе области (стороны прямоугольника).</summary>
        private static bool IsOnBorder(double x, double y) =>
            (x >= 0 && x <= 5 && y >= 3 && y <= 5) ||
            (x >= -7 && x <= 0 && y >= -5 && y <= -3);

        private static double ReadDouble()
        {
            while (true)
            {
                if (double.TryParse(Console.ReadLine(), out double value))
                {
                    return value;
                }

                Console.Write("Ошибка: введите число: ");
            }
        }
    }
}
