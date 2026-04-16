using System;
using System.Drawing;
using System.Text;
/*
Bachelorprojekt 2025
volumetric LED matrix base

Copyright (c) 2026 2026 Moritz Koslowski, Bela Felsch

Permission is granted to use, copy, and modify this source code
for private, non-commercial purposes only.

Any commercial use, distribution, sublicensing, or inclusion
in a commercial product or service is strictly prohibited
without prior written permission from the copyright holder.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
*/

//Dieses Programm wandelt 2D-Bitmaps aus einem Blender-Export, bei welchem die Kamera um das 3D-Objekt
//herumgeschwenkt wurde und Bilder aufgenommen wurden, in ein Spezielles Matrix-Format um, welche von dem 
//Bachelorprojekt, der volumetrischen LED-Matrix, angezeigt werden können.

//filename Spezifikationen:
//bitmap00_00 -> erste Nullen für Bildnummer, zweite Nullen für Frame-Nummer

//settings.init:    line 1: subFramesAmount
//                  line 2: luminanceTreshold

namespace bmp_converter
{
    internal class Program
    {
        static void Main(string[] args)
        {
            while (true)
            {
                Console.Write("Bitmap Converter für Volumetrische LED Matrix");
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.WriteLine(" - mit Helligkeitskompensation!");
                Console.ResetColor();
                Console.WriteLine("Quell-Bitmaps müssen in einem Ordner der wie die .bmp's heißt, gespeichert werden, welche in dem Ordner \"bitmaps\" im Ordner der .exe abgelegt sein muss");
                Console.WriteLine("Ziel-Header Dateien werden im Unterordner \"convertedArrays\" im Ordner der .exe abgelegt.");
                Console.WriteLine("Alle Dateien die von diesem Programm benutzt werden, müssen im Ordner der .exe abgelegt sein.");
                Console.WriteLine("Anzahl der Subframes sowie luminanceTreshold können in settings.init angepasst werden.");
                byte[] matrixLut =
                {
                    196, 197, 198, 199, 212, 213, 214, 215, 228, 229, 230, 231, 244, 245, 246, 247,
                    192, 193, 194, 195, 208, 209, 210, 211, 224, 225, 226, 227, 240, 241, 242, 243,
                    203, 202, 201, 200, 219, 218, 217, 216, 235, 234, 233, 232, 251, 250, 249, 248,
                    207, 206, 205, 204, 223, 222, 221, 220, 239, 238, 237, 236, 255, 254, 253, 252,
                    132, 133, 134, 135, 148, 149, 150, 151, 164, 165, 166, 167, 180, 181, 182, 183,
                    128, 129, 130, 131, 144, 145, 146, 147, 160, 161, 162, 163, 176, 177, 178, 179,
                    139, 138, 137, 136, 155, 154, 153, 152, 171, 170, 169, 168, 187, 186, 185, 184,
                    143, 142, 141, 140, 159, 158, 157, 156, 175, 174, 173, 172, 191, 190, 189, 188,
                    68, 69, 70, 71, 84, 85, 86, 87, 100, 101, 102, 103, 116, 117, 118, 119,
                    64, 65, 66, 67, 80, 81, 82, 83, 96, 97, 98, 99, 112, 113, 114, 115,
                    75, 74, 73, 72, 91, 90, 89, 88, 107, 106, 105, 104, 123, 122, 121, 120,
                    79, 78, 77, 76, 95, 94, 93, 92, 111, 110, 109, 108, 127, 126, 125, 124,
                    4, 5, 6, 7, 20, 21, 22, 23, 36, 37, 38, 39, 52, 53, 54, 55,
                    0, 1, 2, 3, 16, 17, 18, 19, 32, 33, 34, 35, 48, 49, 50, 51,
                    11, 10, 9, 8, 27, 26, 25, 24, 43, 42, 41, 40, 59, 58, 57, 56,
                    15, 14, 13, 12, 31, 30, 29, 28, 47, 46, 45, 44, 63, 62, 61, 60
                };


                byte[][][] luxComp = new byte[][][]
                {
                    new byte[][]
                    {
                        new byte[]
                        {
                            (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF,(byte)0xFF
                        },
                        new byte[]
                        {
                            (byte)0xFF,(byte)0xFF,(byte)0x77,(byte)0xEE,(byte)0xEE,(byte)0x77,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x77,(byte)0xEE,(byte)0xEE,(byte)0x77,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x77,(byte)0xEE,(byte)0xEE,(byte)0x77,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x77,(byte)0xEE,(byte)0xEE,(byte)0x77,(byte)0xFF,(byte)0xFF
                        },
                        new byte[]
                        {
                            (byte)0xFF,(byte)0xFF,(byte)0x33,(byte)0xCC,(byte)0xCC,(byte)0x33,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x33,(byte)0xCC,(byte)0xCC,(byte)0x33,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x33,(byte)0xCC,(byte)0xCC,(byte)0x33,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x33,(byte)0xCC,(byte)0xCC,(byte)0x33,(byte)0xFF,(byte)0xFF
                        },
                        new byte[]
                        {
                            (byte)0xFF,(byte)0xFF,(byte)0x11,(byte)0x88,(byte)0x88,(byte)0x11,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x11,(byte)0x88,(byte)0x88,(byte)0x11,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x11,(byte)0x88,(byte)0x88,(byte)0x11,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x11,(byte)0x88,(byte)0x88,(byte)0x11,(byte)0xFF,(byte)0xFF
                        },
                        new byte[]
                        {
                            (byte)0xFF,(byte)0xFF,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xFF,(byte)0xFF,
                            (byte)0xFF,(byte)0xFF,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xFF,(byte)0xFF
                        },
                        new byte[]
                        {
                            (byte)0x77,(byte)0xEE,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xEE,(byte)0x77,
                            (byte)0x77,(byte)0xEE,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xEE,(byte)0x77,
                            (byte)0x77,(byte)0xEE,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xEE,(byte)0x77,
                            (byte)0x77,(byte)0xEE,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xEE,(byte)0x77
                        },
                        new byte[]
                        {
                            (byte)0x33,(byte)0xCC,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xCC,(byte)0x33,
                            (byte)0x33,(byte)0xCC,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xCC,(byte)0x33,
                            (byte)0x33,(byte)0xCC,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xCC,(byte)0x33,
                            (byte)0x33,(byte)0xCC,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xCC,(byte)0x33
                        },
                        new byte[]
                        {
                            (byte)0x11,(byte)0x88,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x88,(byte)0x11,
                            (byte)0x11,(byte)0x88,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x88,(byte)0x11,
                            (byte)0x11,(byte)0x88,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x88,(byte)0x11,
                            (byte)0x11,(byte)0x88,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x88,(byte)0x11
                        }
                    }
                };


                //Header Datei inkludieren
                string headerPath = Directory.GetCurrentDirectory() + "\\" + "frames.h"; // Speicherort der Header Datei
                bool[] headerArray = new bool[256];
                string[] filePaths = Array.Empty<string>();
                Bitmap[][] bitmaps = Array.Empty<Bitmap[]>();
                int framesAmount = 0;
                int subFramesAmount = 0;
                byte[][][] frame_SubFrame8Bit = Array.Empty<byte[][]>(); // erste stelle Frame, zweite stelle Subframe, dritte stelle Byte Array
                byte[][][][] luxCompSubFrame = Array.Empty<byte[][][]>();
                string Hfilename = "";
                string? filename = "";
                int fileError = 0;
                String destinationFileName = "";
                String destinationFilePath = "";
                double luminanceTreshold = 128;
                string[] settingsLines = { };


                Console.WriteLine("Bitte angeben wie viele Frames vorhanden sind: ");
                while (true)
                {
                    try
                    {
                        framesAmount = int.Parse(Console.ReadLine());
                        if (framesAmount > 0)
                        {
                            break;
                        }
                        Console.WriteLine("Bitte eine gültige Zahl angeben.");
                    }
                    catch
                    {
                        Console.WriteLine("Bitte eine gültige Zahl angeben.");
                    }
                }

                Console.WriteLine("Bitte den Namen der Zieldatei angeben:");
                while (true)
                {
                    destinationFileName = Console.ReadLine();
                    if (destinationFileName.Length > 0)
                    {
                        try
                        {
                            destinationFilePath = "convertedArrays\\" + destinationFileName + "_" + framesAmount + ".h";
                            File.Create(Directory.GetCurrentDirectory() + "\\" + destinationFilePath).Close(); //overwrite in case the file already exists
                            break;
                        }
                        catch
                        {
                            Console.WriteLine("Ungültiger Dateiname.");
                        }
                    }
                    Console.WriteLine("Ungültiger Dateiname.");
                }

                Console.WriteLine("Bitte den .BMP Dateinamen der Quelldateien bzw. Ordnernamen ohne Suffixnummer eingeben: ");

                while (true)
                {
                    filename = Console.ReadLine();
                    if (File.Exists(Directory.GetCurrentDirectory() + "\\bitmaps" + "\\" + filename + "\\" + filename + "00_00.bmp") || File.Exists(Directory.GetCurrentDirectory() + "\\" + filename + "\\" + filename + "00_00.BMP"))
                    {
                        break;
                    }
                    Console.WriteLine("Datei mit dem Namen " + filename + "00_01.bmp wurde nicht gefunden");
                    Console.WriteLine("Bitte erneut versuchen.");
                }

                if (File.Exists("settings.init"))
                {
                    settingsLines = File.ReadAllLines("settings.init");
                    subFramesAmount = int.Parse(settingsLines[0].Split(' ')[0]);
                    luminanceTreshold = int.Parse(settingsLines[1].Split(' ')[0]);
                }

                if (subFramesAmount < 1 || luminanceTreshold > 255 || luminanceTreshold < 0)
                {
                    Console.ForegroundColor = ConsoleColor.Yellow;
                    Console.WriteLine("settings.init ist fehlerhaft oder konnte nicht gefunden werden. Daten müssen manuell eingegeben werden.");
                    Console.ResetColor();
                    Console.WriteLine("Bitte angeben wie viele Subframes vorhanden sind: ");
                    while (true)
                    {
                        try
                        {
                            subFramesAmount = int.Parse(Console.ReadLine());
                            if (subFramesAmount > 0)
                            {
                                break;
                            }
                            Console.WriteLine("Bitte eine gültige Zahl angeben.");
                        }
                        catch
                        {
                            Console.WriteLine("Bitte eine gültige Zahl angeben.");
                        }
                    }

                    Console.WriteLine("Bitte LuminanceTreshold angeben (1 - 255) (desto höher, desto weniger sensitiv auf Helligkeit): ");
                    while (true)
                    {
                        try
                        {
                            luminanceTreshold = int.Parse(Console.ReadLine());
                            if (luminanceTreshold > 0 && luminanceTreshold < 256)
                            {
                                break;
                            }
                            Console.WriteLine("Bitte eine gültige Zahl angeben.");
                        }
                        catch
                        {
                            Console.WriteLine("Bitte eine gültige Zahl angeben.");
                        }
                    }

                }

                Console.WriteLine();
                frame_SubFrame8Bit = new Byte[framesAmount][][];
                bitmaps = new Bitmap[framesAmount][];

                for (int i = 0; i < framesAmount; i++)
                {
                    Console.WriteLine("Frame: " + i);
                    bitmaps[i] = new Bitmap[subFramesAmount];
                    frame_SubFrame8Bit[i] = new Byte[subFramesAmount][];

                    for (int j = 0; j < subFramesAmount; j++)
                    {
                        frame_SubFrame8Bit[i][j] = new Byte[32];
                        Console.WriteLine("Subframe: " + j);
                        string filePath = "";
                        if (j < 10)
                        {
                            if (i < 10)
                            {
                                Hfilename = filename + "0" + i + "_0" + j;
                                filePath = Directory.GetCurrentDirectory() + "\\bitmaps" + "\\" + filename + "\\" + Hfilename;
                            }
                            else
                            {
                                Hfilename = filename + i + "_0" + j;
                                filePath = Directory.GetCurrentDirectory() + "\\bitmaps" + "\\" + filename + "\\" + Hfilename;
                            }
                        }
                        else
                        {
                            if (i < 10)
                            {
                                Hfilename = filename + "0" + i + "_" + j;
                                filePath = Directory.GetCurrentDirectory() + "\\bitmaps" + "\\" + filename + "\\" + Hfilename;
                            }
                            else
                            {
                                Hfilename = filename + i + "_" + j;
                                filePath = Directory.GetCurrentDirectory() + "\\bitmaps" + "\\" + filename + "\\" + Hfilename;
                            }
                        }
                        try
                        {
                            bitmaps[i][j] = new Bitmap(filePath + ".bmp");
                            int arrayCounter = 0;
                            for (int y = 0; y < 16; y++)
                            {
                                for (int x = 0; x < 16; x++)
                                {
                                    Color c = bitmaps[i][j].GetPixel(x, y);
                                    if (IsBlackByLuminance(c, luminanceTreshold))
                                    {
                                        Console.Write("  ");
                                        headerArray[arrayCounter] = false;
                                    }
                                    else
                                    {
                                        Console.Write("██");
                                        headerArray[arrayCounter] = true;
                                    }
                                    arrayCounter++;
                                }
                                Console.WriteLine();
                            }
                            Console.WriteLine();
                        }
                        catch (Exception ex)
                        {
                            Console.ForegroundColor = ConsoleColor.Red;
                            Console.WriteLine($"Fehler beim Verarbeiten der Datei {filePath}.bmp: {ex.Message}");
                            Console.ResetColor();
                            fileError++;
                        }

                        //apply look-up-table (LUT) for correct pixel order
                        bool[] buffer = new bool[256];
                        for (int k = 0; k < 256; k++)
                        {
                            if (headerArray[k])
                            {
                                buffer[matrixLut[k]] = true;
                            }
                        }

                        //convert boolean array to byte array
                        for (int b = 0; b < 32; b++)
                        {
                            for (int c = 0; c < 8; c++)
                            {
                                if (buffer[8 * b + c])
                                {
                                    frame_SubFrame8Bit[i][j][31 - b] |= (byte)(1 << c);
                                }
                            }
                        }
                    }
                }



                // Allokation passend zur späteren Indizierung [ii][m][jj][k] = [8][frames][subFrames][32]
                luxCompSubFrame = new byte[8][][][];
                for (int ii = 0; ii < 8; ii++)
                {
                    luxCompSubFrame[ii] = new byte[framesAmount][][];
                    for (int m = 0; m < framesAmount; m++)
                    {
                        luxCompSubFrame[ii][m] = new byte[subFramesAmount][];
                        for (int jj = 0; jj < subFramesAmount; jj++)
                        {
                            luxCompSubFrame[ii][m][jj] = new byte[32];
                        }
                    }
                }



                //generate 8 brightness corrected frames
                for (int ii = 0; ii < 8; ii++) //8 for the 8 brightness compensated frames
                {
                    for (int jj = 0; jj < subFramesAmount; jj++)
                    {
                        for (int k = 0; k < 32; k++)
                        {
                            for (int m = 0; m < framesAmount; m++)
                            {
                                luxCompSubFrame[ii][m][jj][k] = (byte)(frame_SubFrame8Bit[m][jj][k] & luxComp[0][ii][k]);
                            }
                        }
                    }
                }

                if (fileError > 0)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    if (fileError > 1)
                    {
                        Console.WriteLine(fileError.ToString() + " Dateien wurden nicht gefunden oder konnten nicht verarbeitet werden.");
                    }
                    else
                    {
                        Console.WriteLine(fileError.ToString() + " Datei wurde nicht gefunden oder konnte nicht verarbeitet werden.");
                    }

                    Console.ResetColor();
                    Console.WriteLine("Möchten sie dennoch fortfahren? (y/n)");
                    while (true)
                    {
                        String answer = Console.ReadLine();
                        if (answer.Equals("y"))
                        {
                            break;
                        }
                        else if (answer.Equals("n"))
                        {
                            Environment.Exit(0);
                        }
                        Console.WriteLine("Ungültige Eingabe.");
                    }
                }


                string cBlock = HeaderBlockBuilder.BuildByte4DArrayBlock(
                    luxCompSubFrame,
                    arrayName: destinationFileName + "_" + framesAmount,
                    asHex: true,            // oder false für Dezimal
                    makeConst: true,        // "const uint8_t ..."
                    cType: "uint8_t",       // C-Typ anpassbar
                    innerValuesPerLine: 16  // Lesbarkeit der innersten Ebene
                );
                try
                {
                    File.AppendAllText(destinationFilePath, cBlock, Encoding.UTF8);
                }
                catch (Exception e)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Fataler Fehler: " + e);
                    Console.ResetColor();
                    Console.WriteLine("Drücken sie Enter um das Programm zu beenden." + e);
                    Console.ReadLine();
                    Environment.Exit(0);
                }

                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("Datei " + destinationFileName + ".h wurde gespeichert unter:");
                Console.ResetColor();
                Console.WriteLine(Directory.GetCurrentDirectory() + "\\" + destinationFilePath);
                Console.WriteLine();
                Console.WriteLine("Möchten sie eine weitere Konvertierung ausführen oder das Programm beenden? (y/n)");
                while (true)
                {
                    String answer = Console.ReadLine();
                    if (answer.Equals("y"))
                    {
                        break;
                    }
                    else if (answer.Equals("n"))
                    {
                        Environment.Exit(0);
                    }
                    Console.WriteLine("Ungültige Eingabe.");
                }
            }
        }
        //Überprüfe ob ein Pixel von einer Bitmap schwarz ist (oder dunkel)
        //die LED-Matrix hat nur einen 1-Bit Farbraum
        static bool IsBlackByLuminance(Color c, double threshold)
        {
            // Standard-Luminanzformel (sRGB)
            double luminance = 0.2126 * c.R + 0.7152 * c.G + 0.0722 * c.B;
            return luminance < threshold;
        }

        //keine Ahnung was hier passiert. Das hat Beler mit Chat geschrieben.


        public static class HeaderBlockBuilder
        {
            /// <summary>
            /// Baut den C-Array-Block für ein jagged byte[][][][]:
            /// const uint8_t name[d0][d1][d2][d3] = { ... };
            /// - Ermittelt automatisch die Dimensionen (maximale Länge je Ebene).
            /// - Füllt fehlende Werte (unregelmäßige Längen) mit 0 auf.
            /// - Werte als Hex (0x..) oder dezimal ausgebbar.
            /// - Nur String-Erzeugung; kein Datei-Zugriff.
            /// </summary>
            public static string BuildByte4DArrayBlock(
                byte[][][][] src,
                string arrayName = "array4D",
                bool asHex = true,
                bool makeConst = true,
                string cType = "uint8_t",
                int innerValuesPerLine = 16)
            {
                if (src == null) throw new ArgumentNullException(nameof(src));

                // Dimensionen bestimmen (maximale Längen, da jagged)
                int dim0 = src.Length;
                int dim1 = 0, dim2 = 0, dim3 = 0;

                for (int i = 0; i < dim0; i++)
                {
                    var lvl1 = src[i];
                    if (lvl1 == null) continue;
                    dim1 = Math.Max(dim1, lvl1.Length);

                    for (int j = 0; j < lvl1.Length; j++)
                    {
                        var lvl2 = lvl1[j];
                        if (lvl2 == null) continue;
                        dim2 = Math.Max(dim2, lvl2.Length);

                        for (int k = 0; k < lvl2.Length; k++)
                        {
                            var lvl3 = lvl2[k];
                            if (lvl3 == null) continue;
                            dim3 = Math.Max(dim3, lvl3.Length);
                        }
                    }
                }

                var sb = new StringBuilder(1024);
                string constPrefix = makeConst ? "const " : "";

                sb.AppendLine($"{constPrefix}{cType} {arrayName}[{dim0}][{dim1}][{dim2}][{dim3}] = {{");

                for (int i = 0; i < dim0; i++)
                {
                    sb.AppendLine("    {"); // Ebene 1

                    var lvl1 = (i < src.Length) ? src[i] : null;

                    for (int j = 0; j < dim1; j++)
                    {
                        sb.AppendLine("        {"); // Ebene 2

                        var lvl2 = (lvl1 != null && j < lvl1.Length) ? lvl1[j] : null;

                        for (int k = 0; k < dim2; k++)
                        {
                            var lvl3 = (lvl2 != null && k < lvl2.Length) ? lvl2[k] : null;

                            sb.Append("            { ");

                            for (int l = 0; l < dim3; l++)
                            {
                                byte value = 0;
                                if (lvl3 != null && l < lvl3.Length) value = lvl3[l];

                                if (asHex) sb.Append($"0x{value:X2}");
                                else sb.Append(value.ToString());

                                if (l < dim3 - 1) sb.Append(", ");

                                if (innerValuesPerLine > 0 && l < dim3 - 1 && (l + 1) % innerValuesPerLine == 0)
                                {
                                    sb.AppendLine();
                                    sb.Append("              ");
                                }
                            }

                            sb.Append(" }");
                            if (k < dim2 - 1) sb.Append(",");
                            sb.AppendLine();
                        }

                        sb.Append("        }");
                        if (j < dim1 - 1) sb.Append(",");
                        sb.AppendLine();
                    }

                    sb.Append("    }");
                    if (i < dim0 - 1) sb.Append(",");
                    sb.AppendLine();
                }

                sb.AppendLine("};");
                sb.AppendLine();

                return sb.ToString();
            }
        }
    }
}

