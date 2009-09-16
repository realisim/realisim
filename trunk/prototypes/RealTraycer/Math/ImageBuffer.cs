using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Realisim.Core
{
  public class ImageBuffer
  {
    private ushort _resX;
    private ushort _resY;
    private byte[] _buffer;

    public ImageBuffer()
    {
    }

    public ushort ResX
    {
      get { return _resX; }
    }
    public ushort ResY
    {
      get { return _resY; }
    }
    public byte[] Buffer
    {
      get { return _buffer; }
    }
  }
}
