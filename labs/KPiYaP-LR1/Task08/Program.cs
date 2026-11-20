using System;

namespace Task08
{
    /// <summary>
    /// Задание 8 (вариант 11). Построить таблицу значений функции y = f(x)
    /// для x ∈ [a, b] с шагом h.
    ///
    ///        ⎧ 1 / (x² + 2x - 15),  если -6 &lt; x &lt; 6;
    ///   y =  ⎨ √(x + 8),           если x &lt; -7;
    ///        ⎩ x³,                  если x ≥ 10.
    ///
    /// В остальных точках функция не определена.
    /// </summary>
    internal static class Program
    {
        // Ограничение на число строк таблицы: защищает от зацикливания,
        // если шаг h слишком мал для диапазона [a; b].
        private const int MaxRows = 10_000;

        private static void Main()
        {
            Console.Write("Введите a: ");
            double a = ReadDouble();
            Console.Write("Введите b: ");
            double b = ReadDouble();
            Console.Write("Введите шаг h: ");
            double h = ReadDouble();

            if (h <= 0)
            {
                Console.WriteLine("Ошибка: шаг h должен быть положительным.");
                return;
            }

            if (a > b)
            {
                Console.WriteLine("Ошибка: должно выполняться a ≤ b.");
                return;
            }

            Console.WriteLine();
            Console.WriteLine($"{"№",2}  {"x",8}  {"y",12}");
            Console.WriteLine(new string('-', 26));

            int index = 1;
            // Небольшая поправка EPS компенсирует погрешность накопления шага.
            const double eps = 1e-9;
            for (double x = a; x <= b + eps && index <= MaxRows; x += h, index++)
            {
                string y = TryEvaluate(x, out double value)
                    ? value.ToString("F4")
                    : "не опр.";

                Console.WriteLine($"{index,2}  {x,8:F2}  {y,12}");
            }
        }

        /// <summary>Вычисляет значение функции. Возвращает false, если функция не определена.</summary>
        private static bool TryEvaluate(double x, out double y)
        {
            y = 0;

            if (x < -7)
            {
                y = Math.Sqrt(x + 8);
                return true;
            }

            if (x > -6 && x < 6)
            {
                double denominator = x * x + 2 * x - 15; // (x - 3)(x + 5)
                if (Math.Abs(denominator) < 1e-12)
                {
                    return false; // x = 3 или x = -5 — деление на ноль
                }

                y = 1 / denominator;
                return true;
            }

            if (x >= 10)
            {
                y = x * x * x;
                return true;
            }

            return false; // промежутки [-7; -6] и [6; 10) не входят в область определения
        }

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
