using System;

namespace Task07
{
    /// <summary>
    /// Задание 7 (вариант 11). Вывести на экран матрицу 6 × 6,
    /// у которой на главной диагонали стоит номер строки, а остальные элементы равны 1:
    ///
    ///   1 1 1 1 1 1
    ///   1 2 1 1 1 1
    ///   1 1 3 1 1 1
    ///   1 1 1 4 1 1
    ///   1 1 1 1 5 1
    ///   1 1 1 1 1 6
    /// </summary>
    internal static class Program
    {
        private const int Size = 6;

        private static void Main()
        {
            for (int row = 1; row <= Size; row++)
            {
                for (int column = 1; column <= Size; column++)
                {
                    Console.Write(row == column ? row : 1);
                    Console.Write(' ');
                }

                Console.WriteLine();
            }
        }
    }
}
