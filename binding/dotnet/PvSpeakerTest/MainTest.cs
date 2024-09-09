/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

using Pv;

namespace PvSpeakerTest
{
    [TestClass]
    public class MainTest
    {
        private static readonly int SAMPLE_RATE = 22050;
        private static readonly int BITS_PER_SAMPLE = 16;
        private static readonly int BUFFER_SIZE_SECS = 1;

        [TestMethod]
        public void TestInit()
        {
            PvSpeaker speaker = PvSpeaker.Create(SAMPLE_RATE, BITS_PER_SAMPLE, BUFFER_SIZE_SECS, deviceIndex: 0);
            Assert.IsNotNull(speaker);
            Assert.IsTrue(speaker.SampleRate == SAMPLE_RATE);
            Assert.IsTrue(speaker.BitsPerSample == BITS_PER_SAMPLE);
            Assert.IsTrue(speaker.BufferSizeSecs == BUFFER_SIZE_SECS);
            Assert.IsFalse(string.IsNullOrEmpty(speaker.SelectedDevice));
            Assert.IsFalse(string.IsNullOrEmpty(speaker.Version));
            speaker.Dispose();
        }

        [TestMethod]
        public void TestStartStop()
        {
            using (PvSpeaker speaker = PvSpeaker.Create(SAMPLE_RATE, BITS_PER_SAMPLE, BUFFER_SIZE_SECS, deviceIndex: 0))
            {
                Assert.IsFalse(speaker.IsStarted);
                speaker.Start();
                Assert.IsTrue(speaker.IsStarted);

                string tmpFile = "tmp.wav";
                speaker.WriteToFile(tmpFile);
                
                int bytesPerSample = BITS_PER_SAMPLE / 8;
                int pcmBytesLength = BUFFER_SIZE_SECS * (SAMPLE_RATE * bytesPerSample);
                byte[] pcmBytes = new byte[pcmBytesLength];
                
                int writtenLength = speaker.Write(pcmBytes);
                Assert.IsNotNull(writtenLength);
                Assert.AreEqual(writtenLength, BUFFER_SIZE_SECS * SAMPLE_RATE);
                
                int flushedLength = speaker.Flush();
                Assert.IsNotNull(flushedLength);
                Assert.AreEqual(flushedLength, 0);
                
                speaker.Stop();
                Assert.IsFalse(speaker.IsStarted);
                
                Assert.IsTrue(File.Exists(tmpFile));
                File.Delete(tmpFile);
            }
        }

        [TestMethod]
        public void TestWriteFlush()
        {
            using (PvSpeaker speaker = PvSpeaker.Create(SAMPLE_RATE, BITS_PER_SAMPLE, BUFFER_SIZE_SECS, deviceIndex: 0))
            {
                speaker.Start();
        
                int bytesPerSample = BITS_PER_SAMPLE / 8;
                int pcmBytesLength = (BUFFER_SIZE_SECS + 1) * (SAMPLE_RATE * bytesPerSample);
                byte[] pcmBytes = new byte[pcmBytesLength];
                
                int writtenLength = speaker.Write(pcmBytes);
                Assert.IsNotNull(writtenLength);
                Assert.AreEqual(writtenLength, BUFFER_SIZE_SECS * SAMPLE_RATE);
                
                int flushedLength = speaker.Flush(pcmBytes);
                Assert.IsNotNull(flushedLength);
                Assert.AreEqual(flushedLength, pcmBytesLength / bytesPerSample);
                
                speaker.Stop();
            }
        }
        
        [TestMethod]
        public void TestGetAudioDevices()
        {
            string[] devices = PvSpeaker.GetAvailableDevices();

            Assert.IsNotNull(devices);
            Assert.IsTrue(devices.Length >= 0);
            if (devices.Length > 0)
            {
                for (int i = 0; i < devices.Length; i++)
                {
                    Assert.IsNotNull(devices[i]);
                }
            }
        }
    }
}