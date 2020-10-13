using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using StringStackWrapper;

namespace Lab4GUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            StringStack stack = new StringStack(13);

            string input = "hello!";
            foreach (char value in input)
                stack.Push(value);
            
            char mid = stack.Read();

            string res = "";
            for (int i = 0; i < input.Length; i++)
                res = stack.Pop() + res;
            
        }
    }
}
