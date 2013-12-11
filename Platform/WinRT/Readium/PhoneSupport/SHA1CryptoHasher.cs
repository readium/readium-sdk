//
//  SHA1CryptoHasher.cs
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
using System.Security.Cryptography;

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public class SHA1CryptoHasher
    {
        private SHA1Managed _tx;

        public SHA1CryptoHasher()
        {
            _tx = new SHA1Managed();
        }

        public bool CanReuseTransform
        {
            get { return _tx.CanReuseTransform; }
        }

        public bool CanTransformMultipleBlocks
        {
            get { return _tx.CanTransformMultipleBlocks; }
        }

        public byte[] Hash
        {
            get { return _tx.Hash; }
        }

        public int HashSize
        {
            get { return _tx.HashSize; }
        }

        public int InputBlockSize
        {
            get { return _tx.InputBlockSize; }
        }

        public int OutputBlockSize
        {
            get { return _tx.OutputBlockSize; }
        }

        public void Clear()
        {
            _tx.Clear();
        }

        public byte[] ComputeHash(byte[] bytes)
        {
            return _tx.ComputeHash(bytes);
        }

        public byte[] ComputeHash(byte[] bytes, int offset, int count)
        {
            return _tx.ComputeHash(bytes, offset, count);
        }

        public void Initialize()
        {
            _tx.Initialize();
        }

        public int TransformBlock(byte[] inputBuffer, int inputOffset, int inputCount, byte[] outputBuffer, int outputOffset)
        {
            return _tx.TransformBlock(inputBuffer, inputOffset, inputCount, outputBuffer, outputOffset);
        }

        public byte[] TransformFinalBlock(byte[] inputBuffer, int inputOffset, int inputCount)
        {
            return _tx.TransformFinalBlock(inputBuffer, inputOffset, inputCount);
        }

        public void Dispose()
        { }
    }
}