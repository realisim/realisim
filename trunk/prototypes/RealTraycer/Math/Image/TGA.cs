using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;

namespace Realisim.Core.Image
{
  public class TGA
  {
    private ImageBuffer _image;

    public TGA(ImageBuffer image)
    {
      _image = image;
    }

    public void SaveToFile(string folderFileName)
    {
      using (FileStream fs = new FileStream(folderFileName, FileMode.OpenOrCreate, FileAccess.Write))
      {
        using (BinaryWriter bw = new BinaryWriter(fs))
        {
          byte[] headBuf = new byte[18];

          // --- create the header (TARGA file) ---
          headBuf[0] = 0; // ID field lenght.
          headBuf[1] = 0; // Color map type.
          headBuf[2] = 2; // Image type: true color, uncompressed.
          headBuf[3] = 0; // First color map entry.
          headBuf[4] = 0;
          headBuf[5] = 0; // Color map lenght.
          headBuf[6] = 0;
          headBuf[7] = 0; // Color map entry size.
          headBuf[8] = 0; // Image X origin.
          headBuf[9] = 0;
          headBuf[10] = 0; // Image Y origin.
          headBuf[11] = 0;
          headBuf[12] = (byte)(_image.ResX & 0x00FF);
          headBuf[13] = (byte)(_image.ResX >> 8);
          headBuf[14] = (byte)(_image.ResY & 0x00FF);
          headBuf[15] = (byte)(_image.ResY >> 8);
          headBuf[16] = 24; // Bits per pixel.
          headBuf[17] = 0; // Image descriptor bits.

          bw.Write(headBuf);
          bw.Write(_image.Buffer);

          bw.Close();
        }
        fs.Close();
      }
    }

    

  }
}