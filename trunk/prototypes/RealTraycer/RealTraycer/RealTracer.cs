using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Realisim.Core;
using Realisim.Core.Image;

namespace RealTraycer
{
  public class RTMsgEventArgs : EventArgs
  {
    public string Message { get; set; }

    public RTMsgEventArgs(string message)
    {
      Message = message;
    }
  }

  public class RTPixelEventArgs : EventArgs
  {
    public Color Col { get; set; }
    public int X { get; set; }
    public int Y { get; set; }

    public RTPixelEventArgs(Color col, int x, int y)
    {
      Col = col;
      X = x;
      Y = y;
    }
  }

  public class RealTracer
  {
    #region Events
    public event EventHandler<RTMsgEventArgs> OnMessage;
    public void DoOnMessage(string msg)
    {
      // Copy to a temporary variable to be thread-safe.
      EventHandler<RTMsgEventArgs> temp = OnMessage;
      if(temp != null)
      {
        temp(this, new RTMsgEventArgs(msg));
      }
    }

    public event EventHandler<RTPixelEventArgs> OnPixel;
    public void DoOnPixel(Color col, int x, int y)
    {
      // Copy to a temporary variable to be thread-safe.
      EventHandler<RTPixelEventArgs> temp = OnPixel;
      if(temp != null)
      {
        temp(this, new RTPixelEventArgs(col, x, y));
      }
    }
    #endregion

    private DataReader _dataReader;
    private TGA _image;
    private string _inputFile = string.Empty;
    private Dictionary<Triangle, List<Triangle>> _frontFaceTriangle = new Dictionary<Triangle, List<Triangle>>();

    public RealTracer(string inputFile)
    {
      _inputFile = inputFile;
    }

    public void Load()
    {
      DoOnMessage("Start - loading input file. (calculate preprocess needed)");
      _dataReader = new DataReader(_inputFile);
      DoOnMessage("End - loading input file.");
    }

    public void Trace()
    {
      DoOnMessage("Calculate form factor.");
      CalculateFormFactor();
    }

    private void CalculateFormFactor()
    {
      List<Triangle> trglLst;

      for(int i = 0; i < Data.Instance.Triangles.Count; i++)
      {
        trglLst = new List<Triangle>();
        _frontFaceTriangle.Add(Data.Instance.Triangles[i], trglLst);

        for(int j = i; j < Data.Instance.Triangles.Count; j++)
        {
          if(Data.Instance.Triangles[i].IsFrontFacing(Data.Instance.Triangles[j]))
          {            
            trglLst.Add(Data.Instance.Triangles[j]);
          }
        }
      }
    }

  }
}
