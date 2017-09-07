using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CreateCSVFile
{
    public partial class form1 : Form
    {
        public form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dia = new FolderBrowserDialog();
            if (dia.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = dia.SelectedPath;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            StringBuilder str = new StringBuilder();
            //获取文件夹下的所有文件夹
            //C#遍历指定文件夹中的所有文件 
            DirectoryInfo TheFolder = new DirectoryInfo(textBox1.Text);
            //遍历文件夹
            List<string> paths = new List<string>();
            foreach (DirectoryInfo NextFolder in TheFolder.GetDirectories())
            {
                paths.Add(NextFolder.FullName);
            }
            paths.Sort();
            int index = 0;
            //遍历文件
            foreach (string NextFile in paths)
            {
                DirectoryInfo file = new DirectoryInfo(NextFile);
                FileInfo[] files = file.GetFiles();
                //this.listBox2.Items.Add(NextFile.Name);
                foreach (var item in files)
                {
                    str.AppendLine(item.FullName+";"+index);
                    textBox2.AppendText("生成"+ item.FullName+"\n");
                }
                index++;
            }
            String filepath = textBox1.Text + "\\face.csv";
            bool here = Directory.Exists(filepath);
            if (here)
            {
                //消息框中需要显示哪些按钮，此处显示“确定”和“取消”
                MessageBoxButtons messButton = MessageBoxButtons.OKCancel;
                //"确定要退出吗？"是对话框的显示信息，"退出系统"是对话框的标题
                DialogResult dr = MessageBox.Show("文件已存在，是否覆盖？", "覆盖", messButton);
                if (dr == DialogResult.OK)//如果点击“确定”按钮
                {
                    File.WriteAllText(filepath, str.ToString());
                }
                else//如果点击“取消”按钮
                {
                    return;
                }
            }
            File.WriteAllText(filepath, str.ToString());
            textBox2.AppendText("全部完成");
            MessageBox.Show("完成");
        }
    }
}
