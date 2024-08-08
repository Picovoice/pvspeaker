//
// Copyright 2024 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

import PvSpeakerStatus from "./pv_speaker_status_t";

class PvSpeakerStatusOutOfMemoryError extends Error {}
class PvSpeakerStatusInvalidArgumentError extends Error {}
class PvSpeakerStatusInvalidStateError extends Error {}
class PvSpeakerStatusBackendError extends Error {}
class PvSpeakerStatusDeviceAlreadyInitializedError extends Error {}
class PvSpeakerStatusDeviceNotInitializedError extends Error {}
class PvSpeakerStatusIOError extends Error {}
class PvSpeakerStatusRuntimeError extends Error {}

function pvSpeakerStatusToException(status: PvSpeakerStatus, errorMessage: string): Error {
  switch (status) {
    case PvSpeakerStatus.OUT_OF_MEMORY:
      return new PvSpeakerStatusOutOfMemoryError(errorMessage);
    case PvSpeakerStatus.INVALID_ARGUMENT:
      return new PvSpeakerStatusInvalidArgumentError(errorMessage);
    case PvSpeakerStatus.INVALID_STATE:
      return new PvSpeakerStatusInvalidStateError(errorMessage);
    case PvSpeakerStatus.BACKEND_ERROR:
      return new PvSpeakerStatusBackendError(errorMessage);
    case PvSpeakerStatus.DEVICE_ALREADY_INITIALIZED:
      return new PvSpeakerStatusDeviceAlreadyInitializedError(errorMessage);
    case PvSpeakerStatus.DEVICE_NOT_INITIALIZED:
      return new PvSpeakerStatusDeviceNotInitializedError(errorMessage);
    case PvSpeakerStatus.IO_ERROR:
      return new PvSpeakerStatusIOError(errorMessage);
    case PvSpeakerStatus.RUNTIME_ERROR:
      return new PvSpeakerStatusRuntimeError(errorMessage);
    default:
      // eslint-disable-next-line
      console.warn(`Unknown error code: ${status}`);
      return new Error(errorMessage);
  }
}

export default pvSpeakerStatusToException;
