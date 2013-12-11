//
//  IXmlElement.cs
//  ReadiumPhoneSupport
//
//  Created by Jim Dovey on 2013-12-03.
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
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Xml;
using System.Xml.Linq;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Storage;
using System.Threading;
using System.Net;

namespace ReadiumPhoneSupport
{
    internal class RequestState
    {
        public WebRequest request;
        public WebResponse response;
        public Stream responseStream;
        public ManualResetEvent gotStream;
        public RequestState()
        {
            request = null;
            response = null;
            responseStream = null;
            gotStream = new ManualResetEvent(false);
        }
    }

    internal class AsyncUriLoader
    {
        private RequestState state;

        public AsyncUriLoader()
        { }

        ~AsyncUriLoader()
        {
            state.response.Close();
        }

        public Stream GetStreamForUri(Uri uri)
        {
            try
            {
                WebRequest req = WebRequest.Create(uri);
                if (req == null)
                    return null;

                state = new RequestState();
                state.request = req;
                IAsyncResult asyncResult = req.BeginGetResponse(new AsyncCallback(ResponseCallback), state);
                state.gotStream.WaitOne();

                return state.responseStream;
            }
            catch (Exception)
            {
                return null;
            }
        }

        public static void ResponseCallback(IAsyncResult asyncResult)
        {
            try
            {
                RequestState state = (RequestState)asyncResult.AsyncState;
                WebRequest req = state.request;
                state.response = req.EndGetResponse(asyncResult);
                state.responseStream = state.response.GetResponseStream();
                state.gotStream.Set();
            }
            catch (Exception)
            {
            }
        }
    }
}