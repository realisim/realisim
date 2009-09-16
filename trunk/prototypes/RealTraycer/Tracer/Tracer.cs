using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using RealTraycer;

namespace Tracer
{
  public partial class Tracer : Form
  {
    private RealTracer _realTracer;

    public Tracer()
    {
      InitializeComponent();
    }

    private void toolStripButtonFile_Click(object sender, EventArgs e)
    {
      OpenFileDialog ofd = new OpenFileDialog();

      ofd.Filter = "REO files (*.reo)|*.reo|All files (*.*)|*.*";
      ofd.Multiselect = false;

      if (ofd.ShowDialog() == DialogResult.OK)
      {
        try
        {
          if(_realTracer != null)
          {
            _realTracer.OnMessage -= new EventHandler<RTMsgEventArgs>(_realTracer_OnMessage);
            _realTracer.OnPixel -= new EventHandler<RTPixelEventArgs>(_realTracer_OnPixel);
            _realTracer = null;
          }

          _realTracer = new RealTracer(ofd.FileName);

          _realTracer.OnMessage += new EventHandler<RTMsgEventArgs>(_realTracer_OnMessage);
          _realTracer.OnPixel += new EventHandler<RTPixelEventArgs>(_realTracer_OnPixel);

          _realTracer.Load();
          _realTracer.Trace();
        }
        catch (Exception ex)
        {
          MessageBox.Show(string.Format("Error open file: {0}, {1}", ex.Message, ex.StackTrace));
        }
      }
    }

    private void _realTracer_OnPixel(object sender, RTPixelEventArgs e)
    {
      //throw new NotImplementedException();
    }

    private void _realTracer_OnMessage(object sender, RTMsgEventArgs e)
    {
      //throw new NotImplementedException();
      MessageBox.Show(e.Message);
    }

  }
}
