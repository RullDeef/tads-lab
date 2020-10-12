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

namespace Lab4GUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            str_st_create();
        }

        [DllImport("D:\\Dev\\TiSD\\Lab4\\Debug\\StringStackLib.dll")]
        public static extern void str_st_create();
    }
}
