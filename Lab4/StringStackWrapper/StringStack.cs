using System;
using System.Runtime.InteropServices;

namespace StringStackWrapper
{
    [StructLayout(LayoutKind.Sequential)]
    public struct __StringStack
    {
        public ulong __alloc_sz;
        public ulong __curr_sz;
        public IntPtr __data;
        public IntPtr top;
    }
    
    // wrapper class
    public class StringStack
    {
        public __StringStack __stack;

        public StringStack()
        {
            __stack = __StringStackMethods.strStCreate();
        }

        ~StringStack()
        {
            __StringStackMethods.strStFree(ref __stack);
        }

        public StringStack(ulong size)
        {
            __stack = __StringStackMethods.strStCreate();

            __StringStackMethods.strStResize(ref __stack, size);
        }

        public void Push(char value)
        {
            int status = __StringStackMethods.strStPush(ref __stack, value);
            if (status != 0)
                throw new OverflowException();
        }

        public char Pop()
        {
            char value = '\0';
            int status = __StringStackMethods.strStPop(ref __stack, ref value);
            if (status != 0)
                throw new OverflowException();
            return value;
        }

        public char Read()
        {
            char value = '\0';
            int status = __StringStackMethods.strStRead(ref __stack, ref value);
            if (status != 0)
                throw new OverflowException();
            return value;
        }

        private static class __StringStackMethods
        {
            private const string DLL_LOC = @"D:\Dev\TiSD\Lab4\Debug\StringStackLib.dll";

            [DllImport(DLL_LOC, CharSet = CharSet.Ansi, CallingConvention=CallingConvention.Cdecl)]
            extern public static __StringStack strStCreate();

            [DllImport(DLL_LOC, CharSet=CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            extern public static void strStFree(ref __StringStack stack);

            [DllImport(DLL_LOC, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            extern public static void strStResize(ref __StringStack stack, ulong size);

            [DllImport(DLL_LOC, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            extern public static int strStPush(ref __StringStack stack, char value);

            [DllImport(DLL_LOC, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            extern public static int strStPop(ref __StringStack stack, ref char value);

            [DllImport(DLL_LOC, CharSet=CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
            extern public static int strStRead(ref __StringStack stack, ref char value);
        }
    }
}
