/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using System;

namespace Pv
{
    public class PvSpeakerException : Exception
    {
        public PvSpeakerException() { }

        public PvSpeakerException(string message) : base(message) { }

    }

    public class PvSpeakerMemoryException : PvSpeakerException
    {
        public PvSpeakerMemoryException() { }

        public PvSpeakerMemoryException(string message) : base(message) { }
    }

    public class PvSpeakerInvalidArgumentException : PvSpeakerException
    {
        public PvSpeakerInvalidArgumentException() { }

        public PvSpeakerInvalidArgumentException(string message) : base(message) { }
    }

    public class PvSpeakerInvalidStateException : PvSpeakerException
    {
        public PvSpeakerInvalidStateException() { }

        public PvSpeakerInvalidStateException(string message) : base(message) { }
    }

    public class PvSpeakerBackendException : PvSpeakerException
    {
        public PvSpeakerBackendException() { }

        public PvSpeakerBackendException(string message) : base(message) { }
    }

    public class PvSpeakerDeviceAlreadyInitializedException : PvSpeakerException
    {
        public PvSpeakerDeviceAlreadyInitializedException() { }

        public PvSpeakerDeviceAlreadyInitializedException(string message) : base(message) { }
    }

    public class PvSpeakerDeviceNotInitializedException : PvSpeakerException
    {
        public PvSpeakerDeviceNotInitializedException() { }

        public PvSpeakerDeviceNotInitializedException(string message) : base(message) { }
    }


    public class PvSpeakerIOException : PvSpeakerException
    {
        public PvSpeakerIOException() { }

        public PvSpeakerIOException(string message) : base(message) { }
    }

    public class PvSpeakerRuntimeException : PvSpeakerException
    {
        public PvSpeakerRuntimeException() { }

        public PvSpeakerRuntimeException(string message) : base(message) { }
    }
}