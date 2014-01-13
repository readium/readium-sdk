//
//  AESCryptoEngine.cs
//  ReadiumPhoneSupport
//
//  Created by Jim Dovey on 2013-12-02.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Security.Cryptography;
using Windows.Storage.Streams;

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public sealed class AESCryptoEngine : PhoneSupportInterfaces.ICryptoEngine
    {
        private ICryptoTransform _cryptor;

        public AESCryptoEngine()
        {
        }

        public bool Initialize(bool encrypt, IBuffer iv, IBuffer key)
        {
            if (_cryptor != null)
                return false;

            using (AesManaged aes = new AesManaged())
            {
                if (encrypt)
                    _cryptor = aes.CreateEncryptor(key.ToArray(), iv.ToArray());
                else
                    _cryptor = aes.CreateDecryptor(key.ToArray(), iv.ToArray());
            }

            return true;
        }

        public bool CanReuseTransform
        {
            get { return _cryptor.CanReuseTransform; }
        }

        public bool CanTransformMultipleBlocks
        {
            get { return _cryptor.CanTransformMultipleBlocks; }
        }

        public int InputBlockSize
        {
            get { return _cryptor.InputBlockSize; }
        }

        public int OutputBlockSize
        {
            get { return _cryptor.OutputBlockSize; }
        }

        public int TransformBlock(IBuffer inputBuffer, int inputOffset, int inputCount, IBuffer outputBuffer, int outputOffset)
        {
            return _cryptor.TransformBlock(inputBuffer.ToArray((uint)inputOffset, inputCount), 0, inputCount, outputBuffer.ToArray(), outputOffset);
        }

        public IBuffer TransformFinalBlock(IBuffer inputBuffer, int inputOffset, int inputCount)
        {
            return _cryptor.TransformFinalBlock(inputBuffer.ToArray((uint)inputOffset, inputCount), 0, inputCount).AsBuffer();
        }

        public void Dispose()
        {
        }
    }
}